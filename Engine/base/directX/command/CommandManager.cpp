#include "CommandManager.h"
#include <cassert>
#include <format>
#include "../Engine/utility/MyUtility.h"
#include "../Engine/base/ImGuiManager.h"

#include "../Engine/object/core/Camera.h"
#include "../Engine/object/core/light/PointLight.h"

#include <Config.h>
namespace RenderingPara = LWP::Config::Rendering;
namespace FPSPara = LWP::Config::FrameRate;

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
}

void CommandManager::SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
	rtv_ = rtv;
	dsv_ = dsv;
	srv_ = srv;

	// コマンド用クラス実態宣言
	mainCommand = std::make_unique<MainCommand>();
	mainCommand->SetDescriptorHeap(rtv_, dsv_, srv_);
	mainCommand->Initialize(device_, dxc_.get(), rootSignature_.Get(), CreateBufferResource(sizeof(IndexInfoStruct) * mainCommand->kMaxIndex));
	shadowCommand = std::make_unique<ShadowMapCommand>();
	shadowCommand->SetDescriptorHeap(rtv_, dsv_, srv_);
	shadowCommand->Initialize(device_, dxc_.get(), rootSignature_.Get(), CreateBufferResource(sizeof(IndexInfoStruct) * shadowCommand->kMaxIndex));

	// グラフィックリソースを作成
	CreateStructuredBufferResources();

	// 描画に必要なデータをセット
	shadowCommand->SetDataPtr(structCountResourceBuffer_->data_, directionLightResourceBuffer_.get(), pointLightResourceBuffer_.get());

	// SRVを登録してからでないとテクスチャが読み込めないので、
	// ここでデフォルトテクスチャを読み込む
	defaultTexture_ = LWP::Resource::LoadTextureLongPath("resources/system/texture/white.png");
}

void CommandManager::PreDraw() {/* -- DrawCallを圧縮したのでそのうち削除 -- */}

void CommandManager::DrawCall() {
	HRESULT hr;

	// コマンドリストの実行とリセットのラムダ式（渡す引数が多いのでこちらで定義し、ラムダを渡す）
	std::function<void()> ExecuteLambda = [&]() {
		// GPUにコマンドリストの実行を行わせる
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
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
	};
	std::function<void()> dummy = [&]() {};

	// コマンドのDrawを呼び出すラムダ式（引数で渡すのは面倒なのでラムダで指定）
	std::function<void(ICommand*)> DrawLambda = [&](ICommand* cmd) {
		cmd->Draw(rootSignature_.Get(), commandList_.Get(), dummy, {
			structCountResourceBuffer_->view_,
			directionLightResourceBuffer_->view_,
			pointLightResourceBuffer_->view_,
			vertexResourceBuffer_->view_,
			cameraResourceBuffer_->view_,
			matrixResourceBuffer_->view_,
			lightVPResourceBuffer_->view_,
			materialResourceBuffer_->view_,
			textureResourceBuffer_->view_,
			directionLightResourceBuffer_->shadowMap_[0].view_,
			pointLightResourceBuffer_->shadowMap_[0].view_
		});
	};

	// シャドウマップの描画
	shadowCommand->InitializePreDraw();
	DrawLambda(shadowCommand.get());
	shadowCommand->End();
	// 本描画
	DrawLambda(mainCommand.get());

	ExecuteLambda();

	// GPUとOSに画面の交換を行うよう通知する
	rtv_->GetSwapChain()->Present(FPSPara::kVsync, 0);	// 垂直同期をする際は左の数字を1にする
}

void CommandManager::PostDraw() {/* -- DrawCallを圧縮したのでそのうち削除 -- */}

void CommandManager::Reset() {
	// 使用量をリセット
	mainCommand->indexResourceBuffer_->usedCount_ = 0;
	shadowCommand->indexResourceBuffer_->usedCount_ = 0;
	pointLightResourceBuffer_->usedCount_ = 0;
	vertexResourceBuffer_->usedCount_ = 0;
	matrixResourceBuffer_->usedCount_ = 0;
	lightVPResourceBuffer_->usedCount_ = 0;
	materialResourceBuffer_->usedCount_ = 0;
}

void CommandManager::SetCameraViewProjection(const Object::Camera* camera) {
	// カメラのViewProjectionをリソースにコピー（現在はメインのカメラにしか対応していないので、そのうちマルチレンダリングに対応させる）
	cameraResourceBuffer_->data_[0] = camera->GetViewProjectionMatrix3D();
	cameraResourceBuffer_->data_[1] = camera->GetViewProjectionMatrix2D();
}
void CommandManager::SetPointLightData(const Object::PointLight* light, const Math::Matrix4x4* viewProjections) {
	// 点光源のデータをリソースにコピー
	PointLightStruct newData{};

	newData.color = light->color.GetVector4();	// ライトの色
	newData.position = light->transform.GetWorldPosition();	// ライトのワールド座標
	newData.intensity = light->intensity;		// 輝度
	newData.radius = light->radius;			// ライトの届く最大距離
	newData.decay = light->decay;			// 減衰率
	// データを登録
	pointLightResourceBuffer_->data_[pointLightResourceBuffer_->usedCount_++] = newData;

	// viewProjectionも登録
	for (int i = 0; i < 6; i++) {
		lightVPResourceBuffer_->data_[lightVPResourceBuffer_->usedCount_++] = viewProjections[i];	// 透視射影行列
	}
}
int CommandManager::CreateTextureResource(const DirectX::ScratchImage& image) {
	textureResourceBuffer_->resource_.push_back(CreateBufferResource(image.GetMetadata()));
	UploadTextureData(image);
	return textureResourceBuffer_->usedCount_++;
}

void CommandManager::SetDrawData(Primitive::IPrimitive* primitive) {
	// 最大数を超えていないかチェック
	assert(mainCommand->indexResourceBuffer_->usedCount_ < mainCommand->kMaxIndex);
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
		mainCommand->indexResourceBuffer_->data_[mainCommand->indexResourceBuffer_->usedCount_++] = IndexInfoStruct{
			startIndexNum + primitive->indexes[i],
			cameraVP,
			worldMatrix,
			material,
			tex2d
		};


		// シャドウマップにも描画！
		if (primitive->material.enableLighting) {
			shadowCommand->indexResourceBuffer_->data_[shadowCommand->indexResourceBuffer_->usedCount_++] = IndexInfoStruct{
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
#if _DEBUG //debug時のみ
	ImGui::Begin("PrimitiveCommandManager");
	ImGui::ColorEdit4("color", &directionLightResourceBuffer_->data_->color_.x);
	ImGui::DragFloat3("rotation", &directionLightResourceBuffer_->rotation_.x, 0.01f);
	ImGui::DragFloat("intensity", &directionLightResourceBuffer_->data_->intensity_, 0.01f);
	ImGui::End();
#endif

	// 回転行列を取得
	Matrix4x4 rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(directionLightResourceBuffer_->rotation_);
	// 正規化された方向ベクトルを取得
	directionLightResourceBuffer_->data_->direction_ = (Vector3{ 0.0f,0.0f,1.0f } * rotateMatrix).Normalize();
	// ライトの向きの逆ベクトルがtranslation
	Vector3 v = -1 * directionLightResourceBuffer_->data_->direction_;
	Matrix4x4 translateMatrix = Matrix4x4::CreateTranslateMatrix(v);
	// Viewを計算
	Matrix4x4 viewMatrix = (rotateMatrix * translateMatrix).Inverse();

	//// 回転の向きから正規化された向きを取得
	//lightResourceBuffer_->data_->direction_ = lightResourceBuffer_->data_->direction_.Normalize();

	//// ビュープロジェクション行列を生成
	//// ライトの向きの逆ベクトルを計算
	//Math::Vector3 lightDirection = -1 * lightResourceBuffer_->data_->direction_;

	//// ライトの位置（これは平行光源なので向きを逆にしたベクトル）
	//Math::Vector3 lightPosition = -1 * lightDirection;

	//// ライトの上向きベクトル（仮にy軸を上向きとする）
	//Math::Vector3 up = Math::Vector3(0.0f, 1.0f, 0.0f);

	//// ライトの向きを注視点にするビュー行列
	//Math::Matrix4x4 viewMatrix = Math::Matrix4x4::CreateLookAtMatrix(
	//	lightPosition,                   // 視点（ライトの位置）
	//	{0.0f,0.0f,0.0f},  // 注視点（ライトの位置 + ライトの向き）
	//	up                               // 上向きベクトル
	//);

	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, 10240.0f * RenderingPara::kShadowMapResolutionScale, 10240.0f * RenderingPara::kShadowMapResolutionScale, -5000.0f, 5000.0f);
	Matrix4x4 viewportMatrix = Matrix4x4::CreateViewportMatrix(0.0f, 0.0f, 1024.0f * RenderingPara::kShadowMapResolutionScale, 1024.0f * RenderingPara::kShadowMapResolutionScale, 0.0f, 1.0f);
	lightVPResourceBuffer_->data_[lightVPResourceBuffer_->usedCount_++] = viewMatrix * projectionMatrix * viewportMatrix;
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
	// RootParameter作成。複数設定できるように配列。今回は結果12つ
	D3D12_ROOT_PARAMETER rootParameters[12] = {};
	// テクスチャ用サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers[3] = {};
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

	// 構造体のカウント
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// PixelとVertexで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド
	// 平行光源
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// PixelとVertexで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;						// レジスタ番号0とバインド
	// 点光源
	D3D12_DESCRIPTOR_RANGE pointLightDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	pointLightDesc[0].BaseShaderRegister = 1; // レジスタ番号は1
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[3].DescriptorTable.pDescriptorRanges = pointLightDesc; // Tabelの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(pointLightDesc); // Tableで利用する数



	// ** VertexShaderで使うデータ ** //

	// 頂点データ
	D3D12_DESCRIPTOR_RANGE vertexDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	vertexDesc[0].BaseShaderRegister = 2; // レジスタ番号は2
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[4].DescriptorTable.pDescriptorRanges = vertexDesc; // Tabelの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(vertexDesc); // Tableで利用する数

	// カメラのViewProjection
	D3D12_DESCRIPTOR_RANGE cameraDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	cameraDesc[0].BaseShaderRegister = 3; // レジスタ番号は3
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[5].DescriptorTable.pDescriptorRanges = cameraDesc; // Tabelの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(cameraDesc); // Tableで利用する数

	// WorldTransform
	D3D12_DESCRIPTOR_RANGE wtfDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	wtfDesc[0].BaseShaderRegister = 4; // レジスタ番号は4
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[6].DescriptorTable.pDescriptorRanges = wtfDesc; // Tabelの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = _countof(wtfDesc); // Tableで利用する数

	// lightのviewProjection
	D3D12_DESCRIPTOR_RANGE lightDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	lightDesc[0].BaseShaderRegister = 5; // レジスタ番号は5
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = lightDesc; // Tabelの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(lightDesc); // Tableで利用する数


	// ** PixelShaderで使うデータ ** //

	// マテリアル
	D3D12_DESCRIPTOR_RANGE materialDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	materialDesc[0].BaseShaderRegister = 2; // レジスタ番号は2
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// DescriptorTableを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[8].DescriptorTable.pDescriptorRanges = materialDesc; // Tabelの中身の配列を指定
	rootParameters[8].DescriptorTable.NumDescriptorRanges = _countof(materialDesc); // Tableで利用する数

#pragma region テクスチャ実装
	// Samplerの設定
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号は0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// テクスチャ
	D3D12_DESCRIPTOR_RANGE textureDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	textureDesc[0].BaseShaderRegister = 0; // レジスタ番号は0（スペースが違うので）
	textureDesc[0].RegisterSpace = 1; // スペースは1
	textureDesc[0].NumDescriptors = kMaxTexture;	// 最大数を定義
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[9].DescriptorTable.pDescriptorRanges = textureDesc; // Tabelの中身の配列を指定
	rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(textureDesc); // Tableで利用する数
#pragma endregion

#pragma region 平行光源のシャドウマップ
	// Samplerの設定
	staticSamplers[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // バイアスをかけて線形補間
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数を設定
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[1].ShaderRegister = 1; // レジスタ番号は1
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// シャドウマップ
	D3D12_DESCRIPTOR_RANGE dirShadowDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	dirShadowDesc[0].BaseShaderRegister = 0; // レジスタ番号は0（スペースが違うので）
	dirShadowDesc[0].RegisterSpace = 2; // スペースは2
	dirShadowDesc[0].NumDescriptors = RenderingPara::kMaxDirectionLight;	// 最大数を定義
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[10].DescriptorTable.pDescriptorRanges = dirShadowDesc; // Tabelの中身の配列を指定
	rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(dirShadowDesc); // Tableで利用する数
#pragma endregion

#pragma region 点光源のシャドウマップ
	// Samplerの設定
	staticSamplers[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; // バイアスをかけて線形補間
	staticSamplers[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // 0~1の範囲外をリピート
	staticSamplers[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数を設定
	staticSamplers[2].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[2].ShaderRegister = 2; // レジスタ番号は2
	staticSamplers[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// シャドウマップ
	D3D12_DESCRIPTOR_RANGE pointShadowDesc[1] = { descRange[0] };	// DescriptorRangeを作成
	pointShadowDesc[0].BaseShaderRegister = 0; // レジスタ番号は0（スペースが違うので）
	pointShadowDesc[0].RegisterSpace = 3; // スペースは2
	pointShadowDesc[0].NumDescriptors = RenderingPara::kMaxPointLight * 6;	// 最大数を定義
	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[11].DescriptorTable.pDescriptorRanges = pointShadowDesc; // Tabelの中身の配列を指定
	rootParameters[11].DescriptorTable.NumDescriptorRanges = _countof(pointShadowDesc); // Tableで利用する数
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

void CommandManager::CreateStructuredBufferResources() {
	// 共通のSRV用Desc
	D3D12_SHADER_RESOURCE_VIEW_DESC commonDesc{};
	commonDesc.Format = DXGI_FORMAT_UNKNOWN;
	commonDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	commonDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	commonDesc.Buffer.FirstElement = 0;
	commonDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// 構造体のカウント
	structCountResourceBuffer_ = std::make_unique<StructCountResourceBuffer>();
	structCountResourceBuffer_->resource_ = CreateBufferResource(sizeof(StructCount));
	structCountResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&structCountResourceBuffer_->data_));
	structCountResourceBuffer_->view_ = structCountResourceBuffer_->resource_->GetGPUVirtualAddress();
	// 平行光源
	directionLightResourceBuffer_ = std::make_unique<DirectionLightResourceBuffer>();
	directionLightResourceBuffer_->resource_ = CreateBufferResource(sizeof(DirectionalLightStruct) * RenderingPara::kMaxDirectionLight);
	directionLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&directionLightResourceBuffer_->data_));
	directionLightResourceBuffer_->view_ = directionLightResourceBuffer_->resource_->GetGPUVirtualAddress();
	// シャドウマップも作る
	directionLightResourceBuffer_->shadowMap_ = new DirectionShadowMapStruct[RenderingPara::kMaxDirectionLight];
	directionLightResourceBuffer_->shadowMap_[0].resource_ = dsv_->CreateDirectionShadowMap(&directionLightResourceBuffer_->shadowMap_[0].dsvIndex_, &directionLightResourceBuffer_->shadowMap_[0].view_);
	// 平行光源だけ今は生成を変える
	directionLightResourceBuffer_->data_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	directionLightResourceBuffer_->data_->intensity_ = 0.0f;
	directionLightResourceBuffer_->usedCount_ = 1;
	directionLightResourceBuffer_->rotation_ = { 1.57f,0.0f,0.0f };

	// 点光源
	pointLightResourceBuffer_ = std::make_unique<PointLightResourceBuffer>();
	pointLightResourceBuffer_->resource_ = CreateBufferResource(sizeof(PointLightStruct) * RenderingPara::kMaxPointLight);
	pointLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC pointLightDesc = { commonDesc };
	pointLightDesc.Buffer.NumElements = RenderingPara::kMaxPointLight;
	pointLightDesc.Buffer.StructureByteStride = sizeof(PointLightStruct);
	pointLightResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(pointLightResourceBuffer_->resource_.Get(), &pointLightDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
	// シャドウマップも作る
	pointLightResourceBuffer_->shadowMap_ = new PointShadowMapStruct[RenderingPara::kMaxPointLight];
	for (int i = 0; i < RenderingPara::kMaxPointLight; i++) {
		pointLightResourceBuffer_->shadowMap_[i].resource_ = dsv_->CreatePointShadowMap(pointLightResourceBuffer_->shadowMap_[i].dsvIndex_, &pointLightResourceBuffer_->shadowMap_[i].view_);
	}

	// 頂点データ
	vertexResourceBuffer_ = std::make_unique<VertexResourceBuffer>();
	vertexResourceBuffer_->resource_ = CreateBufferResource(sizeof(VertexStruct) * kMaxVertex);
	vertexResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC vertexDesc = { commonDesc };
	vertexDesc.Buffer.NumElements = kMaxVertex;
	vertexDesc.Buffer.StructureByteStride = sizeof(VertexStruct);
	vertexResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(vertexResourceBuffer_->resource_.Get(), &vertexDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
	// カメラのビュープロジェクション用
	cameraResourceBuffer_ = std::make_unique<MatrixResourceBuffer>();
	cameraResourceBuffer_->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4) * kMaxCameraVP);
	cameraResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC cameraDesc = { commonDesc };
	cameraDesc.Buffer.NumElements = kMaxCameraVP;
	cameraDesc.Buffer.StructureByteStride = sizeof(Math::Matrix4x4);
	cameraResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(cameraResourceBuffer_->resource_.Get(), &cameraDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
	// WorldTransformデータ
	matrixResourceBuffer_ = std::make_unique<MatrixResourceBuffer>();
	matrixResourceBuffer_->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4) * kMaxMatrix);
	matrixResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC matrixDesc = { commonDesc };
	matrixDesc.Buffer.NumElements = kMaxMatrix;
	matrixDesc.Buffer.StructureByteStride = sizeof(Math::Matrix4x4);
	matrixResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(matrixResourceBuffer_->resource_.Get(), &matrixDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
	// lightのviewProjectionデータ
	lightVPResourceBuffer_ = std::make_unique<MatrixResourceBuffer>();
	lightVPResourceBuffer_->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4) * RenderingPara::kMaxShadowMap);
	lightVPResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&lightVPResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC lightDesc = { commonDesc };
	lightDesc.Buffer.NumElements = RenderingPara::kMaxShadowMap;
	lightDesc.Buffer.StructureByteStride = sizeof(Math::Matrix4x4);
	lightVPResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(lightVPResourceBuffer_->resource_.Get(), &lightDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));

	// マテリアルデータ
	materialResourceBuffer_ = std::make_unique<MaterialResourceBuffer>();
	materialResourceBuffer_->resource_ = CreateBufferResource(sizeof(MaterialStruct) * kMaxMaterial);
	materialResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&materialResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC materialDesc = { commonDesc };
	materialDesc.Buffer.NumElements = kMaxMaterial;
	materialDesc.Buffer.StructureByteStride = sizeof(MaterialStruct);
	materialResourceBuffer_->view_ = srv_->GetGPUHandle(srv_->GetCount());
	device_->CreateShaderResourceView(materialResourceBuffer_->resource_.Get(), &materialDesc, srv_->GetCPUHandle(srv_->GetAndIncrement()));
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
	UINT srvIndex = srv_->GetAndIncrement();
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = srv_->GetCPUHandle(srvIndex);
	// 初めてのテクスチャ生成ならviewを保存
	if (textureResourceBuffer_->usedCount_ == 0) {
		textureResourceBuffer_->view_ = srv_->GetGPUHandle(srvIndex);
	}
	// SRVの生成
	device_->CreateShaderResourceView(textureResourceBuffer_->resource_[textureResourceBuffer_->usedCount_].Get(), &srvDesc, textureSRVHandleCPU);
}