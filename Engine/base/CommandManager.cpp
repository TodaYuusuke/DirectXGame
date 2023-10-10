#include "CommandManager.h"
#include <cassert>
#include <format>
#include "../utility/MyUtility.h"
#include "../base/ImGuiManager.h"
#include "../object/core/Camera.h"

#include "../../Adapter/Adapter.h"

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

void CommandManager::Initialize(DirectXCommon* directXCommon) {
	directXCommon_ = directXCommon;

	InitializeDXC();
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

	// デフォルトテクスチャを読み込み
	LWP::Resource::LoadTexture("dummy.png");	
	defaultTexture_ = LWP::Resource::LoadTexture("white.png");
}

void CommandManager::Reset() {
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
	ID3D12GraphicsCommandList* commandList = directXCommon_->GetCommandList();

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	commandList->SetGraphicsRootSignature(pipelineSet_->rootSignature_.Get());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// PSOを設定
	commandList->SetPipelineState(pipelineSet_->graphicsPipelineState_[static_cast<int>(primitive->material.fillMode)].Get());

	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		vertexResourceBuffer_->vertexData_[vertexResourceBuffer_->usedVertexCount_ + i] = primitive->vertices[i];
		if(primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			vertexResourceBuffer_->vertexData_[vertexResourceBuffer_->usedVertexCount_ + i].color_ = primitive->commonColor->GetVector4();
	}
	commandList->IASetVertexBuffers(0, 1, &vertexResourceBuffer_->vertexBufferView_);	// VBVを設定
	// Indexを登録
	for (int i = 0; i < primitive->GetIndexCount(); i++) {
		vertexResourceBuffer_->indexData_[vertexResourceBuffer_->usedIndexCount_ + i] = primitive->indexes[i];
	}
	commandList->IASetIndexBuffer(&vertexResourceBuffer_->indexBufferView_);	// IBVを設定

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

	// 平行光源のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, lightBuffer_->lightResource_->GetGPUVirtualAddress());


	// 描画！
	commandList->DrawIndexedInstanced(primitive->GetIndexCount(), 1, vertexResourceBuffer_->usedIndexCount_, vertexResourceBuffer_->usedVertexCount_, 0);

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

#pragma region PipelineSet

#pragma region PSO生成関連

void CommandManager::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数設定できるように配列。今回は結果5つなので長さ5の配列
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
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
	// 平行光源
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;						// レジスタ番号1とバインド

	// DescriptorRangeを作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	descriptionRootSignature.pParameters = rootParameters;					// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイオリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// テクスチャ
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTabelを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange; // Tabelの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = directXCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet_->rootSignature_));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	//errorBlob->Release();
}
D3D12_INPUT_LAYOUT_DESC CommandManager::CreateInputLayout() {
	// 頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	return inputLayoutDesc;
}
D3D12_BLEND_DESC CommandManager::CreateBlendState() {
	// すべての色要素を書き込む
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 透明度のブレンドを設定
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	return blendDesc;
}
D3D12_RASTERIZER_DESC CommandManager::CreateRasterizerState() {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 埋め立てで設定
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	
	return rasterizerDesc;
}
IDxcBlob* CommandManager::CreateVertexShader() {
	// シェーダーをコンパイルする
	IDxcBlob* vertexShaderBlob{};
	vertexShaderBlob = CompileShader(L"./Engine/resources/shaders/Object3d.VS.hlsl", L"vs_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(vertexShaderBlob != nullptr);
	return vertexShaderBlob;
}
IDxcBlob* CommandManager::CreatePixelShader() {
	// シェーダーをコンパイルする
	IDxcBlob* pixelShaderBlob{};
	pixelShaderBlob = CompileShader(L"./Engine/resources/shaders/Object3d.PS.hlsl", L"ps_6_0", dxc_->dxcUtils_.Get(), dxc_->dxcCompiler_.Get(), dxc_->includeHandler_.Get());
	assert(pixelShaderBlob != nullptr);
	return pixelShaderBlob;
}
D3D12_DEPTH_STENCIL_DESC CommandManager::CreateDepthStencilState() {
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true; // Depthの機能を有効化する
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // 書き込みします
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 比較関数はLessEqual（近ければ描画される）
	return depthStencilDesc;
}

#pragma endregion

void CommandManager::CreateGraphicsPipeLineState() {
	HRESULT hr = S_FALSE;

	pipelineSet_ = std::make_unique<PipelineSet>();

	CreateRootSignature();

	IDxcBlob* vertexShader = CreateVertexShader();
	IDxcBlob* pixelShader = CreatePixelShader();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = pipelineSet_->rootSignature_.Get();	// RootSignature
	graphicsPipelineStateDesc.InputLayout = CreateInputLayout();		// InputLayout
	graphicsPipelineStateDesc.BlendState = CreateBlendState();			// BlendState
	graphicsPipelineStateDesc.RasterizerState = CreateRasterizerState();	// RasterizerState
	graphicsPipelineStateDesc.VS = { vertexShader->GetBufferPointer(),vertexShader->GetBufferSize() };	// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShader->GetBufferPointer(),pixelShader->GetBufferSize() };	// PixelShader
	graphicsPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）	
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineSet_->graphicsPipelineState_[1]));
	assert(SUCCEEDED(hr));
	// ワイヤーフレームモードも生成
	graphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	hr = directXCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineSet_->graphicsPipelineState_[0]));
	assert(SUCCEEDED(hr));

	vertexShader->Release();
	pixelShader->Release();
}

#pragma endregion

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
	hr = directXCommon_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
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
	hr = directXCommon_->GetDevice()->CreateCommittedResource(
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

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = directXCommon_->GetSRVCPUHandle(usedTextureCount_);
	textureResource_[usedTextureCount_]->view_ = directXCommon_->GetSRVGPUHandle(usedTextureCount_);
	// SRVの生成
	directXCommon_->GetDevice()->CreateShaderResourceView(textureResource_[usedTextureCount_]->resource_.Get(), &srvDesc, textureSRVHandleCPU);
}

IDxcBlob* CommandManager::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {

	/*-- 1.hlslファイルを読む --*/

	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読まなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	/*-- 2.Compileする --*/

	LPCWSTR arguments[] = {
		filePath.c_str(),			// コンパイル対象のhlslファイル名
		L"-E",L"main",				// エントリーポイントの指定、基本的にmain以外にはしない
		L"-T",profile,				// ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",						// 最適化を外しておく
		L"-Zpr",					// メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,		// 読み込んだファイル
		arguments,					// コンパイルオプション
		_countof(arguments),		// コンパイルオプションの数
		includeHandler,				// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	// コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));


	/*-- 3.警告・エラーがでていないか確認する --*/

	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}


	/*-- 4.Compile結果を受け取って返す --*/

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"CompileSucceeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}