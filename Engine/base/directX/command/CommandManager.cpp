#include "CommandManager.h"
#include <cassert>
#include <format>
#include "../Engine/utility/MyUtility.h"
#include "../Engine/base/ImGuiManager.h"
#include "../Engine/object/core/Camera.h"

#include <Adapter.h>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

void CommandManager::Initialize(ID3D12Device* device) {
	HRESULT hr = S_FALSE;
	device_ = device;

	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));	// コマンドキューの生成がうまくいかなかったので起動できない
	// コマンドアロケーターを生成する
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));	// コマンドアロケーターの生成がうまくいかなかったので起動できない
	// コマンドリストを生成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));	// コマンドリストの生成がうまくいかなかったので起動できない

	// 初期値0でFenceを作る
	fenceVal_ = 0;
	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// シェーダーコンパイラ生成
	InitializeDXC();
	// RootSignature生成
	CreateRootSignature();
	//CreateShadowRS();
}

void CommandManager::SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
	rtv_ = rtv;
	dsv_ = dsv;
	srv_ = srv;

	// コマンド用クラス実態宣言
	cmds_.push_back(new MainCommand());
	cmds_.push_back(new ShadowMapCommand());
	for (int i = 0; i < cmds_.size(); i++) {
		cmds_[i]->SetDescriptorHeap(rtv_, dsv_, srv_);
		cmds_[i]->Initialize(device_, dxc_.get(), rootSignature_.Get(), CreateBufferResource(sizeof(IndexInfoStruct) * cmds_[i]->kMaxIndex));
	}

	// グラフィックリソースを作成
	CreateStructuredBufferResources();

	// SRVを登録してからでないとテクスチャが読み込めないので、
	// ここでデフォルトテクスチャを読み込む
	defaultTexture_ = LWP::Resource::LoadTexture("white.png");
}

void CommandManager::PreDraw() {

}

void CommandManager::DrawCall() {
	HRESULT hr;

	// レンダリング回数ループする
	for (size_t i = cmds_.size() - 1; i >= 0; i--) {
		// ポインタで受け取る
		ID3D12GraphicsCommandList* commandList = commandList_.Get();

		// 描画前処理
		cmds_[i]->PreDraw(commandList);

		// RootSignatureを設定。PSOに設定してるけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature_.Get());
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// PSOを設定
		commandList->SetPipelineState(cmds_[i]->GetPSOState());

		// ディスクリプタテーブルを登録
		// 0 ... バッファーのインデックス
		// 1 ... 平行光源
		// 2 ... 頂点データ
		// 3 ... カメラのviewProjection
		// 4 ... WorldTransform
		// 5 ... マテリアル
		// 6 ... シャドウマップ
		// 7 ... テクスチャ
		commandList->SetGraphicsRootDescriptorTable(0, cmds_[i]->indexResourceBuffer_->view_);
		commandList->SetGraphicsRootConstantBufferView(1, lightResourceBuffer_->view_);
		commandList->SetGraphicsRootDescriptorTable(2, vertexResourceBuffer_->view_);
		commandList->SetGraphicsRootDescriptorTable(3, cameraResourceBuffer_->view_);
		commandList->SetGraphicsRootDescriptorTable(4, matrixResourceBuffer_->view_);
		commandList->SetGraphicsRootDescriptorTable(5, materialResourceBuffer_->view_);
		commandList->SetGraphicsRootDescriptorTable(6, dsv_->GetShadowView());
		commandList->SetGraphicsRootDescriptorTable(7, textureResourceBuffer_->view_);
		// 全三角形を１つのDrawCallで描画
		commandList->DrawInstanced(3, cmds_[i]->indexResourceBuffer_->usedCount_ / 3, 0, 0);

		// 描画後処理（MainRenderingの時のみPostDrawは後回し）
		if (i == 0) { break; }

		cmds_[i]->PostDraw(commandList);


		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		// GPUにコマンドリストの実行を行わせる
		commandQueue_->ExecuteCommandLists(1, commandLists);

		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue_->Signal(fence_.Get(), ++fenceVal_);
		if (fence_->GetCompletedValue() != fenceVal_) {
			HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(event != nullptr);
			fence_->SetEventOnCompletion(fenceVal_, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		// 次のコマンドリストを準備
		hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
	}
}

void CommandManager::PostDraw() {
	// 描画後処理
	cmds_[0]->PostDraw(commandList_.Get());

	// - コマンドリストをすべてCloseした後 - //

	// コマンドリストを配列化
	//ID3D12CommandList* commandLists[] = { shadowMapCommands_->GetList(), mainCommands_->GetList() };
	//for (int i = 0; i < 2; i++) {
	//	// GPUにコマンドリストの実行を行わせる
	//	commandQueue_->ExecuteCommandLists(1, &commandLists[i]);

	//	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	//	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	//	if (fence_->GetCompletedValue() != fenceVal_) {
	//		HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
	//		assert(event != nullptr);
	//		fence_->SetEventOnCompletion(fenceVal_, event);
	//		WaitForSingleObject(event, INFINITE);
	//		CloseHandle(event);
	//	}
	//}
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	// GPUにコマンドリストの実行を行わせる
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(event != nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	

	// GPUとOSに画面の交換を行うよう通知する
	rtv_->GetSwapChain()->Present(1, 0);
}

void CommandManager::Reset() {
	HRESULT hr;
	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

	for (int i = 0; i < cmds_.size(); i++) {
		cmds_[i]->indexResourceBuffer_->usedCount_ = 0;
	}
	vertexResourceBuffer_->usedCount_ = 0;
	matrixResourceBuffer_->usedCount_ = 0;
	materialResourceBuffer_->usedCount_ = 0;
}

void CommandManager::SetCameraViewProjection(const Object::Camera* camera) {
	cameraResourceBuffer_->data_[0] = camera->GetViewProjectionMatrix3D();
	cameraResourceBuffer_->data_[1] = camera->GetViewProjectionMatrix2D();
}
int CommandManager::CreateTextureResource(const DirectX::ScratchImage& image) {
	textureResourceBuffer_->resource_.push_back(CreateBufferResource(image.GetMetadata()));
	UploadTextureData(image);
	return textureResourceBuffer_->usedCount_++;
}

void CommandManager::SetDrawData(Primitive::IPrimitive* primitive) {
	// 最大数を超えていないかチェック
	assert(cmds_[0]->indexResourceBuffer_->usedCount_ < cmds_[0]->kMaxIndex);
	assert(vertexResourceBuffer_->usedCount_ < kMaxVertex);
	assert(matrixResourceBuffer_->usedCount_ < kMaxMatrix);
	assert(materialResourceBuffer_->usedCount_ < kMaxMaterial);

	uint32_t startIndexNum = vertexResourceBuffer_->usedCount_;
	
	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		vertexResourceBuffer_->data_[vertexResourceBuffer_->usedCount_++] = primitive->vertices[i];
		if(primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			vertexResourceBuffer_->data_[vertexResourceBuffer_->usedCount_ - 1].color_ = primitive->commonColor->GetVector4();
	}
	// 使用するカメラのビュープロジェクション行列をセット
	uint32_t cameraVP = primitive->isUI;
	// ワールドトランスフォームをデータに登録
	uint32_t worldMatrix = matrixResourceBuffer_->usedCount_;
	matrixResourceBuffer_->data_[matrixResourceBuffer_->usedCount_++] = primitive->transform.GetWorldMatrix();
	// マテリアルをデータに登録
	uint32_t material = materialResourceBuffer_->usedCount_;
	materialResourceBuffer_->data_[materialResourceBuffer_->usedCount_++] = primitive->material;
	// テクスチャのインデックスを貰う
	uint32_t tex2d = defaultTexture_->GetIndex();
	if (primitive->texture != nullptr) {
		tex2d = primitive->texture->GetIndex();
	}


	// Indexの分だけIndexInfoを求める
	for (int i = 0; i < primitive->GetIndexCount(); i++) {
		cmds_[0]->indexResourceBuffer_->data_[cmds_[0]->indexResourceBuffer_->usedCount_++] = IndexInfoStruct{
			startIndexNum + primitive->indexes[i],
			cameraVP,
			worldMatrix,
			material,
			tex2d
		};


		// シャドウマップにも描画！
		if (primitive->material.enableLighting) {
			cmds_[1]->indexResourceBuffer_->data_[cmds_[1]->indexResourceBuffer_->usedCount_++] = IndexInfoStruct{
			startIndexNum + primitive->indexes[i],
			cameraVP,
			worldMatrix,
			material,
			tex2d
			};
		}
	}

}

void CommandManager::ImGui() {
	ImGui::Begin("PrimitiveCommandManager");
	ImGui::ColorEdit4("color", &lightResourceBuffer_->data_->color_.x);
	ImGui::DragFloat3("direction", &lightResourceBuffer_->data_->direction_.x, 0.01f);
	ImGui::DragFloat("intensity", &lightResourceBuffer_->data_->intensity_, 0.01f);
	ImGui::End();

	// 方向を正規化
	lightResourceBuffer_->data_->direction_ = lightResourceBuffer_->data_->direction_.Normalize();

	// ビュープロジェクション行列を生成
	// ライトの向きの逆ベクトルを計算
	Math::Vector3 lightDirection = -1 * lightResourceBuffer_->data_->direction_;

	// ライトの位置（これは平行光源なので向きを逆にしたベクトル）
	Math::Vector3 lightPosition = -1 * lightDirection;

	// ライトの上向きベクトル（仮にy軸を上向きとする）
	Math::Vector3 up = Math::Vector3(0.0f, 1.0f, 0.0f);

	// ライトの向きを注視点にするビュー行列
	Math::Matrix4x4 viewMatrix = Math::Matrix4x4::CreateLookAtMatrix(
		lightPosition,                   // 視点（ライトの位置）
		lightPosition + lightDirection,  // 注視点（ライトの位置 + ライトの向き）
		up                               // 上向きベクトル
	);

	//Matrix4x4 viewMatrix = Matrix4x4::CreateLookAtMatrix({ 0.0f,0.0f,0.0f }, lightResourceBuffer_->data_->direction_, { 0.0f,1.0f,0.0f });
	//Matrix4x4 viewMatrix = Matrix4x4::CreateRotateXYZMatrix(lightResourceBuffer_->data_->direction_).Inverse();
	//WorldTransform transform{ {0.0f,0.0f,0.0f},{1.57f,0.0f,0.0f},{1.0f,1.0f,1.0f} };
	//Matrix4x4 viewMatrix = transform.GetWorldMatrix().Inverse();
	//Matrix4x4 viewMatrix = Matrix4x4::CreateIdentity4x4();
	//Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(0.45f, 1280 / 720, 0.1f, 100.0f);
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, 1024.0f * 3.0f, 1024.0f * 3.0f, 0.0f, 100.0f);
	//Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(1024.0f * -1.5f, 1024.0f * 1.5f, 1024.0f * 1.5f, 1024.0f * 1.5f, 0.0f, 100.0f);
	//Matrix4x4 viewprotMatrix = Matrix4x4::CreateViewportMatrix(1024.0f * -1.5f, 1024.0f * 1.5f, 1024.0f * 1.5f, 1024.0f * 1.5f, 0.0f, 1.0f);
	Matrix4x4 viewprotMatrix = Matrix4x4::CreateViewportMatrix(0.0f, 0.0f, 1024.0f, 1024.0, 0.0f, 1.0f);
	lightResourceBuffer_->data_->viewProjection_ = viewMatrix * projectionMatrix * viewprotMatrix;
}


void CommandManager::InitializeDXC() {
	HRESULT hr = S_FALSE;

	dxc_ = std::make_unique<DXC>();

	// DxcUtilsを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxc_->dxcUtils_));
	assert(SUCCEEDED(hr));
	// DxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc_->dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxc_->dxcUtils_->CreateDefaultIncludeHandler(&dxc_->includeHandler_);
	assert(SUCCEEDED(hr));
}

void CommandManager::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	// RootParameter作成。複数設定できるように配列。今回は結果8つ
	D3D12_ROOT_PARAMETER rootParameters[8] = {};
	// テクスチャ用サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	// 配列用のRangeDesc
	D3D12_DESCRIPTOR_RANGE descRange[1] = {};	// DescriptorRangeを作成
	descRange[0].BaseShaderRegister = 0; // 0から始まる
	descRange[0].NumDescriptors = 1; // 数は1つ
	descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// RootSignatureにrootParametersを登録
	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ
	
	// RootSignatureにサンプラーを登録
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	// ** 両方で使うデータ ** //

	// ストラクチャーバッファーのインデックス
	D3D12_DESCRIPTOR_RANGE indexDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	indexDesc[0].BaseShaderRegister = 0; // レジスタ番号は0
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = indexDesc; // Tabelの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(indexDesc); // Tableで利用する数

	// 平行光源
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// PixelとVertexで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド


	// ** VertexShaderで使うデータ ** //

	// 頂点データ
	D3D12_DESCRIPTOR_RANGE vertexDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	vertexDesc[0].BaseShaderRegister = 1; // レジスタ番号は1
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].DescriptorTable.pDescriptorRanges = vertexDesc; // Tabelの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(vertexDesc); // Tableで利用する数

	// カメラのViewProjection
	D3D12_DESCRIPTOR_RANGE cameraDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	cameraDesc[0].BaseShaderRegister = 2; // レジスタ番号は2
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[3].DescriptorTable.pDescriptorRanges = cameraDesc; // Tabelの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(cameraDesc); // Tableで利用する数

	// WorldTransform
	D3D12_DESCRIPTOR_RANGE wtfDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	wtfDesc[0].BaseShaderRegister = 3; // レジスタ番号は3
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[4].DescriptorTable.pDescriptorRanges = wtfDesc; // Tabelの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(wtfDesc); // Tableで利用する数


	// ** PixelShaderで使うデータ ** //

	// マテリアル
	D3D12_DESCRIPTOR_RANGE materialDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	materialDesc[0].BaseShaderRegister = 1; // レジスタ番号は1
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].DescriptorTable.pDescriptorRanges = materialDesc; // Tabelの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(materialDesc); // Tableで利用する数

#pragma region シャドウマップ実装
	// Samplerの設定
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号は0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// シャドウマップ
	D3D12_DESCRIPTOR_RANGE shadowRange[1] = { descRange[0] };	// DescriptorRangeを作成
	shadowRange[0].BaseShaderRegister = 2; // レジスタ番号は2
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[6].DescriptorTable.pDescriptorRanges = shadowRange; // Tabelの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = _countof(shadowRange); // Tableで利用する数
#pragma endregion

#pragma region テクスチャ実装
	// Samplerの設定
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[1].ShaderRegister = 1; // レジスタ番号は1
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// テクスチャ
	D3D12_DESCRIPTOR_RANGE textureDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	textureDesc[0].BaseShaderRegister = 3; // レジスタ番号は3
	textureDesc[0].NumDescriptors = kMaxTexture;	// 最大数を定義
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[7].DescriptorTable.pDescriptorRanges = textureDesc; // Tabelの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(textureDesc); // Tableで利用する数
#pragma endregion


	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}
/*
void CommandManager::CreateShadowRS() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるように配列。今回は結果2つなので長さ2の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};

	// RootSignatureにrootParametersを登録
	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

	// 定数バッファ（World）
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド

	// 平行光源
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// PixelとVertexで使う
	rootParameters[1].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド


	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet_->shadowRS_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}
*/

void CommandManager::CreateStructuredBufferResources() {
	// 共通のSRV用Desc
	D3D12_SHADER_RESOURCE_VIEW_DESC commonDesc{};
	commonDesc.Format = DXGI_FORMAT_UNKNOWN;
	commonDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	commonDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	commonDesc.Buffer.FirstElement = 0;
	commonDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// 平行光源
	lightResourceBuffer_ = std::make_unique<LightResourceBuffer>();
	lightResourceBuffer_->resource_ = CreateBufferResource(sizeof(DirectionalLight));
	lightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&lightResourceBuffer_->data_));
	lightResourceBuffer_->view_ = lightResourceBuffer_->resource_->GetGPUVirtualAddress();
	// 平行光源だけ今は生成を変える
	lightResourceBuffer_->data_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	lightResourceBuffer_->data_->direction_ = { 0.0f,-1.0f,0.0f };
	lightResourceBuffer_->data_->intensity_ = 1.0f;


	// 頂点データ
	vertexResourceBuffer_ = std::make_unique<VertexResourceBuffer>();
	vertexResourceBuffer_->resource_ = CreateBufferResource(sizeof(VertexStruct) * kMaxVertex);
	vertexResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC vertexDesc = { commonDesc };
	vertexDesc.Buffer.NumElements = kMaxVertex;
	vertexDesc.Buffer.StructureByteStride = sizeof(VertexStruct);
	vertexResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetUsedCount());
	device_->CreateShaderResourceView(vertexResourceBuffer_->resource_.Get(), &vertexDesc, srv_->GetCPUHandle(srv_->GetUsedCount()));
	srv_->AddUsedCount();	// SRV使用数を+1
	// カメラのビュープロジェクション用
	cameraResourceBuffer_ = std::make_unique<MatrixResourceBuffer>();
	cameraResourceBuffer_->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4) * kMaxCameraVP);
	cameraResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC cameraDesc = { commonDesc };
	cameraDesc.Buffer.NumElements = kMaxCameraVP;
	cameraDesc.Buffer.StructureByteStride = sizeof(Math::Matrix4x4);
	cameraResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetUsedCount());
	device_->CreateShaderResourceView(cameraResourceBuffer_->resource_.Get(), &cameraDesc, srv_->GetCPUHandle(srv_->GetUsedCount()));
	srv_->AddUsedCount();	// SRV使用数を+1
	// WorldTransformデータ
	matrixResourceBuffer_ = std::make_unique<MatrixResourceBuffer>();
	matrixResourceBuffer_->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4) * kMaxMatrix);
	matrixResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC matrixDesc = { commonDesc };
	matrixDesc.Buffer.NumElements = kMaxMatrix;
	matrixDesc.Buffer.StructureByteStride = sizeof(Math::Matrix4x4);
	matrixResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetUsedCount());
	device_->CreateShaderResourceView(matrixResourceBuffer_->resource_.Get(), &matrixDesc, srv_->GetCPUHandle(srv_->GetUsedCount()));
	srv_->AddUsedCount();	// SRV使用数を+1

	// マテリアルデータ
	materialResourceBuffer_ = std::make_unique<MaterialResourceBuffer>();
	materialResourceBuffer_->resource_ = CreateBufferResource(sizeof(MaterialStruct) * kMaxMaterial);
	materialResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&materialResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC materialDesc = { commonDesc };
	materialDesc.Buffer.NumElements = kMaxMaterial;
	materialDesc.Buffer.StructureByteStride = sizeof(MaterialStruct);
	materialResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetUsedCount());
	device_->CreateShaderResourceView(materialResourceBuffer_->resource_.Get(), &materialDesc, srv_->GetCPUHandle(srv_->GetUsedCount()));
	srv_->AddUsedCount();	// SRV使用数を+1
	// テクスチャデータ
	textureResourceBuffer_ = std::make_unique<TextureResourceBuffer>();
	//textureResource_->resource_ = CreateBufferResource(sizeof(IndexInfoStrict) * kMaxTexture);
}

ID3D12Resource* CommandManager::CreateBufferResource(size_t size) {
	HRESULT hr = S_FALSE;

	ID3D12Resource* resource;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = size; // リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

ID3D12Resource* CommandManager::CreateBufferResource(const DirectX::TexMetadata& metadata) {
	HRESULT hr = S_FALSE;

	// 1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// 2. 利用するHeapの設定。非常に特殊な運用。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// 3. Resourceを生成する
	ID3D12Resource* resource = nullptr;
	hr = device_->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 初回のResourceState。Textureは基本読むだけ
		nullptr,							// Clear最適地。使わないでnullptr
		IID_PPV_ARGS(&resource)				// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}
void CommandManager::UploadTextureData(const DirectX::ScratchImage& mipImages) {
	HRESULT hr = S_FALSE;

	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		hr = textureResourceBuffer_->resource_[textureResourceBuffer_->usedCount_]->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める（ImGuiとStructuredBufferたちが先頭を使っているので + ）
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = srv_->GetCPUHandle(textureResourceBuffer_->usedCount_ + srv_->GetUsedCount());
	// 初めてのテクスチャ生成ならviewを保存
	if (textureResourceBuffer_->usedCount_ == 0) {
		textureResourceBuffer_->view_ = srv_->GetGPUHandle(textureResourceBuffer_->usedCount_ + srv_->GetUsedCount());
	}
	// SRVの生成
	device_->CreateShaderResourceView(textureResourceBuffer_->resource_[textureResourceBuffer_->usedCount_].Get(), &srvDesc, textureSRVHandleCPU);
}