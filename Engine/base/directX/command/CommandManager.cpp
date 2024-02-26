#include "CommandManager.h"
#include <cassert>
#include <format>
#include "../Engine/utility/MyUtility.h"
#include "../Engine/base/ImGuiManager.h"

#include "../Engine/object/core/Camera.h"
#include "../Engine/object/core/light/DirectionLight.h"
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

void CommandManager::SetDescriptorHeap(HeapManager* manager) {
	heaps_ = manager;

	// コマンド用クラス実態宣言
	
	renderer_ = std::make_unique<Renderer>();
	renderer_->Init(device_, dxc_.get(), heaps_);

	// 本描画
	//mainCommand_ = std::make_unique<MainCommand>();
	//mainCommand_->SetDescriptorHeap(heaps_->rtv(), heaps_->dsv(), heaps_->srv());
	//mainCommand_->Initialize(device_, dxc_.get(), rootSignature_->GetRoot());
	
	// サブ描画
	//for (int i = 0; i < lwpC::Rendering::kMaxMultiWindowRendering; i++) {
	//	subCommands_.push_back(std::make_unique<SubRendering>());
	//	subCommands_.back()->SetDescriptorHeap(heaps_->rtv(), heaps_->dsv(), heaps_->srv());
	//	subCommands_.back()->Initialize(device_, dxc_.get(), rootSignature_->GetRoot());
	//}

	// シャドウマップコマンド用の実体
	for (int i = 0; i < lwpC::Shadow::kMaxShadowMap; i++) {
		shadowCommands_.push_back(std::make_unique<ShadowMapping>());
		shadowCommands_.back()->SetDescriptorHeap(heaps_->rtv(), heaps_->dsv(), heaps_->srv());
		shadowCommands_.back()->Initialize(device_, dxc_.get(), rootSignature_->GetRoot());
	}
	
	// ポストプロセスのコマンド実装
	ppManager_ = std::make_unique<PostProcess::Manager>();
	ppManager_->Init(device_, dxc_.get(), heaps_);

	// グラフィックリソースを作成
	CreateStructuredBufferResources();

	// SRVを登録してからでないとテクスチャが読み込めないので、
	// ここでデフォルトテクスチャを読み込む
	defaultTexture_ = LWP::Resource::LoadTextureLongPath("resources/system/texture/white.png");

	// リソースのViewを登録
	renderer_->SetViewStruct(ViewStruct{
		commonDataResourceBuffer_->view_,
		vertexData_->GetView(),
		transformData_->GetView(),
		materialData_->GetView(),
		directionLightResourceBuffer_->view_,
		pointLightResourceBuffer_->view_,
		heaps_->srv()->GetView(),
		directionLightResourceBuffer_->shadowMap_[0].view_,
		pointLightResourceBuffer_->shadowMap_[0].view_
		});
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
		cmd->Draw(rootSignature_->GetRoot(), commandList_.Get(), {
			commonDataResourceBuffer_->view_,
			vertexData_->GetView(),
			transformData_->GetView(),
			materialData_->GetView(),
			directionLightResourceBuffer_->view_,
			pointLightResourceBuffer_->view_,
			heaps_->srv()->GetView(),
			directionLightResourceBuffer_->shadowMap_[0].view_,
			pointLightResourceBuffer_->shadowMap_[0].view_
		});
	};

	// シャドウマップのDrawCallを積む
	for (int i = 0; i < shadowCount_; i++) {
		DrawLambda(shadowCommands_[i].get());
	}
	// サブ描画
	/*for (int i = 0; i < subCount_; i++) {
		DrawLambda(subCommands_[i].get());
	}*/
	// ポストプロセス実行
	//ppManager_->Draw(commandList_.Get());

	// 描画
	//DrawLambda(mainCommand_.get());
	renderer_->Draw(commandList_.Get());

	// 実行
	ExecuteLambda();

	// GPUとOSに画面の交換を行うよう通知する
	heaps_->rtv()->GetSwapChain()->Present(FPSPara::kVsync, 0);	// 垂直同期をする際は左の数字を1にする
}

void CommandManager::PostDraw() {/* -- DrawCallを圧縮したのでそのうち削除 -- */}

void CommandManager::Reset() {
	// コマンドをリセット
	for (int i = 0; i < shadowCount_; i++) {
		shadowCommands_[i]->Reset();
	}
	shadowCount_ = 0;
	
	//for (int i = 0; i < subCount_; i++) {
	//	subCommands_[i]->Reset();
	//}
	//subCount_ = 0;

	//mainCommand_->Reset();
	renderer_->Reset();

	// 使用量をリセット
	vertexData_->Reset();
	transformData_->Reset();
	commonDataResourceBuffer_->data_->directionLight = 0;
	commonDataResourceBuffer_->data_->pointLight = 0;
	materialData_->Reset();
	directionLightResourceBuffer_->usedCount_ = 0;
	pointLightResourceBuffer_->usedCount_ = 0;

	// ポストプロセスもリセット
	ppManager_->Reset();
}

void CommandManager::SetMainRendering(Object::Camera* camera) {
	// カメラ視点の描画を予約
	//mainCommand_->SetDrawTarget(camera->GetViewProjection());
	renderer_->SetMainRenderTarget(camera);
	//ppManager_->SetRenderData(camera->GetRenderTexture(), camera->cctvEffect);
}
void CommandManager::SetSubRendering(Object::Camera* camera) {
	renderer_->SetSubRenderTarget(camera);
	//ppManager_->SetRenderData(camera->GetRenderTexture(), camera->cctvEffect);
}

void CommandManager::SetDirectionLightData(const Object::DirectionLight* light, const Math::Matrix4x4& viewProjection) {
	DirectionalLightStruct newData{};
	newData.vp = viewProjection;	// viewProjection
	newData.color = light->color.GetVector4();		// ライトの色
	newData.direction = (Vector3{ 0.0f,0.0f,1.0f } * Matrix4x4::CreateRotateXYZMatrix(light->rotation)).Normalize();	// ライトの向き
	newData.intensity = light->intensity;	// 輝度
	// データを登録
	directionLightResourceBuffer_->data_[directionLightResourceBuffer_->usedCount_++] = newData;

	shadowCommands_[shadowCount_++]->SetDrawTarget(
		viewProjection,
		directionLightResourceBuffer_->shadowMap_->resource_.Get(),
		directionLightResourceBuffer_->shadowMap_->dsvIndex_
	);
	// 使用カウント+1
	commonDataResourceBuffer_->data_->directionLight++;
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
	pointLightResourceBuffer_->data_[pointLightResourceBuffer_->usedCount_] = newData;

	// シャドウマップのレンダリングを予約
	for (int i = 0; i < 6; i++) {
		shadowCommands_[shadowCount_++]->SetDrawTarget(
			viewProjections[i],
			pointLightResourceBuffer_->shadowMap_[pointLightResourceBuffer_->usedCount_].resource_.Get(),
			pointLightResourceBuffer_->shadowMap_[pointLightResourceBuffer_->usedCount_].dsvIndex_[i]
		);
	}
	// 使用カウント+1
	pointLightResourceBuffer_->usedCount_++;
	commonDataResourceBuffer_->data_->pointLight++;
}

ID3D12Resource* CommandManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	// 1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	return CreateBufferResource(resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
}

ID3D12Resource* CommandManager::CreateTextureResource(const int width, const int height) {
	// 1. Resourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(width);
	resourceDesc.Height = UINT(height);
	resourceDesc.MipLevels = 0;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearColor;
	clearColor.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearColor.Color[0] = 0.0f;
	clearColor.Color[1] = 1.0f;
	clearColor.Color[2] = 0.0f;
	clearColor.Color[3] = 1.0f;

	return CreateBufferResource(resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearColor);
}

void CommandManager::SetDrawData(Primitive::IPrimitive* primitive) {

	uint32_t startIndexNum = vertexData_->GetCount();
	
	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		VertexStruct ver;
		ver = primitive->vertices[i];
		if(primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			ver.color_ = primitive->commonColor->GetVector4();
		vertexData_->AddData(ver);	// データを追加
	}
	// ワールドトランスフォームをデータに登録
	uint32_t worldMatrix = transformData_->GetCount();
	WTFStruct wtf;
	wtf = primitive->transform;
	transformData_->AddData(wtf);
	// マテリアルをデータに登録
	uint32_t material = materialData_->GetCount();
	MaterialStruct m;
	m = primitive->material;
	materialData_->AddData(m);
	// テクスチャのインデックスを貰う
	uint32_t tex2d = defaultTexture_->GetIndex();
	if (primitive->texture.t != nullptr) {
		tex2d = primitive->texture.t->GetIndex();
	}

	// Indexの分だけIndexInfoを求める
	for (int i = 0; i < primitive->GetIndexCount(); i++) {
		IndexInfoStruct indexInfo = {
			startIndexNum + primitive->indexes[i],
			worldMatrix,
			material,
			tex2d,
			primitive->isUI
		};

		// メインコマンドにデータをセット
		//mainCommand_->SetDrawData(indexInfo);
		renderer_->AddRenderData(indexInfo, primitive->isWireFrame, true);
		// サブにもセット（現状サブカメラにUIはいらないので除外する）
		if (!primitive->isUI) {
			//for (int s = 0; s < subCount_; s++) {
				//subCommands_[s]->SetDrawData(indexInfo);
			renderer_->AddRenderData(indexInfo, primitive->isWireFrame, false);
			//}
		}

		// シャドウマップにも描画！
		if (primitive->material.enableLighting) {
			for (int s = 0; s < shadowCount_; s++) {
				shadowCommands_[s]->SetDrawData(indexInfo);
			}
		}
	}
}

void CommandManager::SetParticleData(Primitive::IPrimitive* primitive, std::vector<ParticleData>& wtfVec) {
	uint32_t startIndexNum = vertexData_->GetCount();

	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		VertexStruct ver;
		ver = primitive->vertices[i];
		if (primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			ver.color_ = primitive->commonColor->GetVector4();
		vertexData_->AddData(ver);	// データを追加
	}
	// マテリアルをデータに登録
	uint32_t material = materialData_->GetCount();
	MaterialStruct m;
	m = primitive->material;
	materialData_->AddData(m);
	// テクスチャのインデックスを貰う
	uint32_t tex2d = defaultTexture_->GetIndex();
	if (primitive->texture.t != nullptr) {
		tex2d = primitive->texture.t->GetIndex();
	}

	// ワールドトランスフォームの分ループ
	for (int w = 0; w < wtfVec.size(); w++) {
		// ワールドトランスフォームをデータに登録
		uint32_t worldMatrix = transformData_->GetCount();
		WTFStruct wtf;
		wtf = wtfVec[w].wtf;
		transformData_->AddData(wtf);

		// Indexの分だけIndexInfoを求める
		for (int i = 0; i < primitive->GetIndexCount(); i++) {
			IndexInfoStruct indexInfo = {
				startIndexNum + primitive->indexes[i],
				worldMatrix,
				material,
				tex2d,
				primitive->isUI
			};

			// メインコマンドにデータをセット
			//mainCommand_->SetDrawData(indexInfo);
			renderer_->AddRenderData(indexInfo, primitive->isWireFrame, true);
			// サブにもセット（現状サブカメラにUIはいらないので除外する）
			if (!primitive->isUI) {
				//for (int s = 0; s < subCount_; s++) {
				//subCommands_[s]->SetDrawData(indexInfo);
				renderer_->AddRenderData(indexInfo, primitive->isWireFrame, false);
				//}
			}

			// シャドウマップにも描画！
			if (primitive->material.enableLighting) {
				for (int s = 0; s < shadowCount_; s++) {
					shadowCommands_[s]->SetDrawData(indexInfo);
				}
			}
		}
	}
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
	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->AddTableParameter(0, SV_All)	// インデックスのデータ
		.AddCBVParameter(0, SV_All)	// 描画に使うViewprojection
		.AddCBVParameter(1, SV_All)	// 全画面で共通のデータ
		.AddTableParameter(1, SV_Vertex)	// 頂点データ
		.AddTableParameter(2, SV_Vertex)	// トランスフォーム
		.AddTableParameter(1, SV_Pixel)	// マテリアル
		.AddTableParameter(2, SV_Pixel)	// 平行光源
		.AddTableParameter(3, SV_Pixel)	// 点光源
		.AddTableParameter(0, SV_Pixel, 1, kMaxTexture)	// テクスチャ
		.AddTableParameter(0, SV_Pixel, 2, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(0, SV_Pixel, 3, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device_);
}

void CommandManager::CreateStructuredBufferResources() {
	// 共通のSRV用Desc
	D3D12_SHADER_RESOURCE_VIEW_DESC commonDesc{};
	commonDesc.Format = DXGI_FORMAT_UNKNOWN;
	commonDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	commonDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	commonDesc.Buffer.FirstElement = 0;
	commonDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;


	// 頂点データ
	vertexData_ = std::make_unique<IStructured<VertexStruct>>(device_, heaps_->srv(), RenderingPara::kMaxVertex);
	// WorldTransformデータ
	transformData_ = std::make_unique<IStructured<WTFStruct>>(device_, heaps_->srv(), RenderingPara::kMaxMatrix);

	// 構造体のカウント
	commonDataResourceBuffer_ = std::make_unique<CommonDataResourceBuffer>();
	commonDataResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(CommonData));
	commonDataResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&commonDataResourceBuffer_->data_));
	commonDataResourceBuffer_->view_ = commonDataResourceBuffer_->resource_->GetGPUVirtualAddress();
	// 2D用のViewProjectionを作成しておく
	commonDataResourceBuffer_->data_->vp2D = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, LWP::Info::GetWindowWidthF(), LWP::Info::GetWindowHeightF(), 0.0f, 100.0f);
	// マテリアルデータ
	materialData_ = std::make_unique<IStructured<MaterialStruct>>(device_, heaps_->srv(), RenderingPara::kMaxMaterial);
	
	// 平行光源
	directionLightResourceBuffer_ = std::make_unique<DirectionLightResourceBuffer>();
	directionLightResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(DirectionalLightStruct) * lwpC::Shadow::Point::kMaxCount);
	directionLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&directionLightResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC dirLightDesc = { commonDesc };
	dirLightDesc.Buffer.NumElements = lwpC::Shadow::Direction::kMaxCount;
	dirLightDesc.Buffer.StructureByteStride = sizeof(DirectionalLightStruct);
	directionLightResourceBuffer_->view_ = heaps_->srv()->GetGPUHandle(heaps_->srv()->GetCount());
	device_->CreateShaderResourceView(directionLightResourceBuffer_->resource_.Get(), &dirLightDesc, heaps_->srv()->GetCPUHandle(heaps_->srv()->GetAndIncrement()));
	// シャドウマップも作る
	directionLightResourceBuffer_->shadowMap_ = new DirectionShadowMapStruct[lwpC::Shadow::Direction::kMaxCount];
	for (int i = 0; i < lwpC::Shadow::Direction::kMaxCount; i++) {
		directionLightResourceBuffer_->shadowMap_[i].resource_ = heaps_->dsv()->CreateDirectionShadowMap(&directionLightResourceBuffer_->shadowMap_[i].dsvIndex_, &directionLightResourceBuffer_->shadowMap_[i].view_);
	}
	/*directionLightResourceBuffer_ = std::make_unique<DirectionLightResourceBuffer>();
	directionLightResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(DirectionalLightStruct) * lwpC::Shadow::Direction::kMaxCount);
	directionLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&directionLightResourceBuffer_->data_));
	directionLightResourceBuffer_->view_ = directionLightResourceBuffer_->resource_->GetGPUVirtualAddress();
	// シャドウマップも作る
	directionLightResourceBuffer_->shadowMap_ = new DirectionShadowMapStruct[lwpC::Shadow::Direction::kMaxCount];
	directionLightResourceBuffer_->shadowMap_[0].resource_ = dsv_->CreateDirectionShadowMap(&directionLightResourceBuffer_->shadowMap_[0].dsvIndex_, &directionLightResourceBuffer_->shadowMap_[0].view_);*/

	// 点光源
	pointLightResourceBuffer_ = std::make_unique<PointLightResourceBuffer>();
	pointLightResourceBuffer_->resource_ = CreateBufferResourceStatic(device_, sizeof(PointLightStruct) * lwpC::Shadow::Point::kMaxCount);
	pointLightResourceBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightResourceBuffer_->data_));
	D3D12_SHADER_RESOURCE_VIEW_DESC pointLightDesc = { commonDesc };
	pointLightDesc.Buffer.NumElements = lwpC::Shadow::Point::kMaxCount;
	pointLightDesc.Buffer.StructureByteStride = sizeof(PointLightStruct);
	pointLightResourceBuffer_->view_ = heaps_->srv()->GetGPUHandle(heaps_->srv()->GetCount());
	device_->CreateShaderResourceView(pointLightResourceBuffer_->resource_.Get(), &pointLightDesc, heaps_->srv()->GetCPUHandle(heaps_->srv()->GetAndIncrement()));
	// シャドウマップも作る
	pointLightResourceBuffer_->shadowMap_ = new PointShadowMapStruct[lwpC::Shadow::Point::kMaxCount];
	for (int i = 0; i < lwpC::Shadow::Point::kMaxCount; i++) {
		pointLightResourceBuffer_->shadowMap_[i].resource_ = heaps_->dsv()->CreatePointShadowMap(pointLightResourceBuffer_->shadowMap_[i].dsvIndex_, &pointLightResourceBuffer_->shadowMap_[i].view_);
	}
}

ID3D12Resource* CommandManager::CreateBufferResource(D3D12_RESOURCE_DESC desc, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clearColor) {
	HRESULT hr = S_FALSE;

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
		&desc,						// Resourceの設定
		state,	// 初回のResourceState。RTVで書き込むのでちょっとちがう設定
		clearColor,							// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource)				// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}