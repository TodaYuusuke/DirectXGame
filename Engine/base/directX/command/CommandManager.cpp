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
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドクラスを生成する
	//shadowMapCommands_ = std::make_unique<ShadowMapCommand>();
	//shadowMapCommands_->Initialize(device_);
	mainCommands_ = std::make_unique<MainCommand>();
	mainCommands_->Initialize(device_);


	// 初期値0でFenceを作る
	fenceVal_ = 0;
	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	pipelineSet_ = std::make_unique<PipelineSet>();
	// シェーダーコンパイラ生成
	InitializeDXC();
	// RootSignature生成
	CreateRootSignature();
	//CreateShadowRS();
	// グラフィックスパイプラインを作成
	CreateGraphicsPipeLineState();

	// 頂点のリソースを作成
	CreateVertexResource();
	// 行列のリソースを作成
	CreateMatrixResource();

	// 平行光源のリソース作成
	lightBuffer_ = std::make_unique<LightBuffer>();
	// リソースを作る。サイズはDirectionalLight 1つ分
	lightBuffer_->lightResource_ = CreateBufferResource(sizeof(DirectionalLight));
	// データを書き込む
	lightBuffer_->light_ = new DirectionalLight();
	// 書き込むためのアドレスを取得
	lightBuffer_->lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightBuffer_->light_));

	lightBuffer_->light_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	lightBuffer_->light_->direction_ = { 0.0f,-1.0f,0.0f };
	lightBuffer_->light_->intensity_ = 1.0f;
}

void CommandManager::SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
	rtv_ = rtv;
	dsv_ = dsv;
	srv_ = srv;

	// コマンドにも登録しておく
	//shadowMapCommands_->SetDescriptorHeap(rtv, dsv, srv);
	mainCommands_->SetDescriptorHeap(rtv, dsv, srv);

	// SRVを登録してからでないとテクスチャが読み込めないので、
	// ここでデフォルトテクスチャを読み込む
	defaultTexture_ = LWP::Resource::LoadTexture("white.png");
}

void CommandManager::PreDraw() {
	ID3D12GraphicsCommandList* commandList = nullptr;
	
	// --- シャドウマップのコマンドの初期設定 --- //

	//shadowMapCommands_->PreDraw(pipelineSet_->shadowRS_.Get());
	//commandList = shadowMapCommands_->GetList();

	//// PSOを登録
	//commandList->SetPipelineState(pipelineSet_->shadowPSO_->state_.Get());
	//// 頂点とインデックスの位置を登録
	//commandList->IASetVertexBuffers(0, 1, &vertexResourceBuffer_->vertexBufferView_);	// VBVを設定
	//commandList->IASetIndexBuffer(&vertexResourceBuffer_->indexBufferView_);	// IBVを設定
	//// 平行光源のCBufferの場所を設定
	//commandList->SetGraphicsRootConstantBufferView(1, lightBuffer_->lightResource_->GetGPUVirtualAddress());


	// --- メインのコマンドの初期設定 --- //

	mainCommands_->PreDraw(pipelineSet_->rootSignature_.Get());
	commandList = mainCommands_->GetList();

	// 頂点とインデックスの位置を登録
	commandList->IASetVertexBuffers(0, 1, &vertexResourceBuffer_->vertexBufferView_);	// VBVを設定
	commandList->IASetIndexBuffer(&vertexResourceBuffer_->indexBufferView_);	// IBVを設定
	// 平行光源のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, lightBuffer_->lightResource_->GetGPUVirtualAddress());
	// シャドウマップの場所を設定
	commandList->SetGraphicsRootDescriptorTable(5, dsv_->GetShadowView());
}

void CommandManager::PostDraw() {
	//shadowMapCommands_->PostDraw();
	mainCommands_->PostDraw();

	
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
	ID3D12CommandList* commandLists[] = { mainCommands_->GetList() };
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
	mainCommands_->Reset();
	//shadowMapCommands_->Reset();

	vertexResourceBuffer_->usedVertexCount_ = 0;
	vertexResourceBuffer_->usedIndexCount_ = 0;
	usedMatrixCount_ = 0;
}

void CommandManager::SetCameraViewProjection(const Object::Camera* camera) {
	*cameraResource_[0]->data_ = camera->GetViewProjectionMatrix3D();
	*cameraResource_[1]->data_ = camera->GetViewProjectionMatrix2D();
}

int CommandManager::CreateMaterialResource() {
	materialResource_.push_back(std::make_unique<MaterialResourceBuffer>());
	int index = static_cast<int>(materialResource_.size() - 1);
	materialResource_[index]->resource_ = CreateBufferResource(sizeof(MaterialStruct));
	materialResource_[index]->resource_->Map(0, nullptr, reinterpret_cast<void**>(&materialResource_[index]->data_));
	materialResource_[index]->view_ = materialResource_[index]->resource_->GetGPUVirtualAddress();
	return index;
}
int CommandManager::CreateTextureResource(const DirectX::ScratchImage& image) {
	textureResource_.push_back(std::make_unique<TextureResourceBuffer>());
	textureResource_[usedTextureCount_]->resource_ = CreateBufferResource(image.GetMetadata());
	UploadTextureData(image);
	return usedTextureCount_++;
}

void CommandManager::Draw(Primitive::IPrimitive* primitive) {
	// 最大数を超えていないかチェック
	assert(vertexResourceBuffer_->usedVertexCount_ < kMaxVertexCount_);
	assert(vertexResourceBuffer_->usedIndexCount_ < kMaxIndexCount_);

	// コマンドを積む
	ID3D12GraphicsCommandList* commandList = mainCommands_->GetList();

	// PSOを設定
	commandList->SetPipelineState(pipelineSet_->pso_[static_cast<int>(primitive->material.fillMode)]->state_.Get());

	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		vertexResourceBuffer_->vertexData_[vertexResourceBuffer_->usedVertexCount_ + i] = primitive->vertices[i];
		if(primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			vertexResourceBuffer_->vertexData_[vertexResourceBuffer_->usedVertexCount_ + i].color_ = primitive->commonColor->GetVector4();
	}
	// Indexを登録
	for (int i = 0; i < primitive->GetIndexCount(); i++) {
		vertexResourceBuffer_->indexData_[vertexResourceBuffer_->usedIndexCount_ + i] = primitive->indexes[i];
	}
	// マテリアルの場所を設定
	*materialResource_[primitive->material.GetIndex()]->data_ = primitive->material;
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[primitive->material.GetIndex()]->view_);
	// カメラのビュープロジェクション行列の場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, cameraResource_[primitive->isUI]->view_);
	// WorldTransformの場所を設定
	*matrixResource_[usedMatrixCount_]->data_ = primitive->transform.GetWorldMatrix();
	commandList->SetGraphicsRootConstantBufferView(2, matrixResource_[usedMatrixCount_]->view_);
	// テクスチャの場所を指定。
	if (primitive->texture != nullptr) {
		commandList->SetGraphicsRootDescriptorTable(3, textureResource_[primitive->texture->GetIndex()]->view_);
	}
	else {
		commandList->SetGraphicsRootDescriptorTable(3, textureResource_[defaultTexture_->GetIndex()]->view_);
	}

	// 描画！
	commandList->DrawIndexedInstanced(primitive->GetIndexCount(), 1, vertexResourceBuffer_->usedIndexCount_, vertexResourceBuffer_->usedVertexCount_, 0);


	// シャドウマップにも描画！
	if (primitive->material.enableLighting) {
		// WorldTransformの場所を設定
		//shadowMapCommands_->GetList()->SetGraphicsRootConstantBufferView(0, matrixResource_[usedMatrixCount_]->view_);
		// 描画！
		//shadowMapCommands_->GetList()->DrawIndexedInstanced(primitive->GetIndexCount(), 1, vertexResourceBuffer_->usedIndexCount_, vertexResourceBuffer_->usedVertexCount_, 0);
	}

	// 使用した頂点とインデックスのカウント
	vertexResourceBuffer_->usedVertexCount_ += primitive->GetVertexCount();
	vertexResourceBuffer_->usedIndexCount_ += primitive->GetIndexCount();
	usedMatrixCount_++;
}

void CommandManager::ImGui() {
	ImGui::Begin("PrimitiveCommandManager");
	ImGui::ColorEdit4("color", &lightBuffer_->light_->color_.x);
	ImGui::DragFloat3("direction", &lightBuffer_->light_->direction_.x, 0.01f);
	ImGui::DragFloat("intensity", &lightBuffer_->light_->intensity_, 0.01f);
	ImGui::End();

	Matrix4x4 viewMatrix = Matrix4x4::CreateRotateXYZMatrix(lightBuffer_->light_->direction_).Inverse();
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(512.0f, 512.0f, 512.0f, 512.0f, 0.0f, 100.0f);
	lightBuffer_->light_->viewProjection_ = viewMatrix* projectionMatrix;
}


void CommandManager::InitializeDXC() {
	HRESULT hr = S_FALSE;

	pipelineSet_->dxc_ = std::make_unique<DXC>();

	// DxcUtilsを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pipelineSet_->dxc_->dxcUtils_));
	assert(SUCCEEDED(hr));
	// DxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pipelineSet_->dxc_->dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = pipelineSet_->dxc_->dxcUtils_->CreateDefaultIncludeHandler(&pipelineSet_->dxc_->includeHandler_);
	assert(SUCCEEDED(hr));
}

void CommandManager::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるように配列。今回は結果5つなので長さ5の配列
	D3D12_ROOT_PARAMETER rootParameters[6] = {};
	// テクスチャ用サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	

	// RootSignatureにrootParametersを登録
	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ
	
	// RootSignatureにサンプラーを登録
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);



	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド
	// 定数バッファ（カメラのViewProjection）
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド
	// 定数バッファ（World）
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド

#pragma region テクスチャ実装

	// テクスチャ用のDescriptorRangeを作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// Samplerの設定
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// テクスチャ
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange; // Tabelの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

#pragma endregion

	// 平行光源
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		// PixelとVertexで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;						// レジスタ番号1とバインド
	
#pragma region シャドウマップ実装

	// シャドウマップ用のDescriptorRangeを作成
	D3D12_DESCRIPTOR_RANGE descriptorRangeShadow[1] = {};
	descriptorRangeShadow[0].BaseShaderRegister = 1; // 0から始まる
	descriptorRangeShadow[0].NumDescriptors = 1; // 数は1つ
	descriptorRangeShadow[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRangeShadow[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// Samplerの設定
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[1].ShaderRegister = 1; // レジスタ番号1を使う
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

	// シャドウマップ
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRangeShadow; // Tabelの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeShadow); // Tableで利用する数

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
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet_->rootSignature_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}

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

void CommandManager::CreateGraphicsPipeLineState() {
	for (int r = 0; r < 2; r++) {
		pipelineSet_->pso_[r] = std::make_unique<PSO>();
		pipelineSet_->pso_[r]->Initialize(device_, pipelineSet_->rootSignature_.Get(), pipelineSet_->dxc_.get(), r, 1, 1);
	}

	// シャドウマップ用のPSO
	//pipelineSet_->shadowPSO_ = std::make_unique<PSO>();
	//pipelineSet_->shadowPSO_->InitializeForShadow(device_, pipelineSet_->shadowRS_.Get(), pipelineSet_->dxc_.get());
}


void CommandManager::CreateVertexResource() {
	// 実体を宣言
	vertexResourceBuffer_ = std::make_unique<VertexResourceBuffer>();

	// 頂点とインデックスのリソースを作成
	vertexResourceBuffer_->vertexResource_ = CreateBufferResource(sizeof(VertexStruct) * kMaxVertexCount_);
	vertexResourceBuffer_->indexResource_ = CreateBufferResource(sizeof(uint32_t) * kMaxIndexCount_);

	// 頂点とインデックスのバッファビューを作成する
	// リソースの先頭アドレスから使う
	vertexResourceBuffer_->vertexBufferView_.BufferLocation = vertexResourceBuffer_->vertexResource_.Get()->GetGPUVirtualAddress();
	vertexResourceBuffer_->indexBufferView_.BufferLocation = vertexResourceBuffer_->indexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexResourceBuffer_->vertexBufferView_.SizeInBytes = sizeof(VertexStruct) * kMaxVertexCount_;
	vertexResourceBuffer_->indexBufferView_.SizeInBytes = sizeof(uint32_t) * kMaxIndexCount_;
	// 1頂点あたりのサイズ
	vertexResourceBuffer_->vertexBufferView_.StrideInBytes = sizeof(VertexStruct);
	// インデックスはuint32_tとする
	vertexResourceBuffer_->indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// 書き込むためのアドレスを取得
	vertexResourceBuffer_->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexResourceBuffer_->vertexData_));
	vertexResourceBuffer_->indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexResourceBuffer_->indexData_));
}
void CommandManager::CreateMatrixResource() {
	for(int i = 0; i < kMaxTransformCount_; i++) {
		matrixResource_[i] = std::make_unique<MatrixResourceBuffer>();
		matrixResource_[i]->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4));
		matrixResource_[i]->resource_->Map(0, nullptr, reinterpret_cast<void**>(&matrixResource_[i]->data_));
		matrixResource_[i]->view_ = matrixResource_[i]->resource_->GetGPUVirtualAddress();
	}

	// 2Dと3Dのカメラビュープロジェクション行列を作る
	for (int i = 0; i < 2; i++) {
		cameraResource_[i] = std::make_unique<MatrixResourceBuffer>();
		cameraResource_[i]->resource_ = CreateBufferResource(sizeof(Math::Matrix4x4));
		cameraResource_[i]->resource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraResource_[i]->data_));
		cameraResource_[i]->view_ = cameraResource_[i]->resource_->GetGPUVirtualAddress();
	}
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
		hr = textureResource_[usedTextureCount_]->resource_->WriteToSubresource(
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

	// SRVを作成するDescriptorHeapの場所を決める（ImGuiが先頭を使っているので+1）
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = srv_->GetCPUHandle(usedTextureCount_ + 2);
	textureResource_[usedTextureCount_]->view_ = srv_->GetGPUHandle(usedTextureCount_ + 2);
	// SRVの生成
	device_->CreateShaderResourceView(textureResource_[usedTextureCount_]->resource_.Get(), &srvDesc, textureSRVHandleCPU);	
}