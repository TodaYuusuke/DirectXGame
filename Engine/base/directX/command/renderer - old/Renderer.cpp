#include "Renderer.h"
#include "object/core/Camera.h"

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;
constexpr int kMaxRendering = lwpC::Rendering::kMaxMultiWindowRendering;	// 最大値を省略

void Renderer::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps) {
	// ポインタをセット
	device_ = device;
	dxc_ = dxc;
	// ヒープ管理クラスをセット
	heaps_ = heaps;

	// メインレンダリングの実態を生成する
	mainRenderer_ = std::make_unique<MainRenderer>();
	mainRenderer_->Init(device, dxc, heaps);

	// サブレンダリングの実体を生成する
	for (int i = 0; i < kMaxRendering; i++) {
		subRenderer_.push_back(std::make_unique<SubRenderer>());
		subRenderer_[i]->Init(device, mainRenderer_->GetRoot(), mainRenderer_->GetPSOSolid(), mainRenderer_->GetPSOWire(), heaps);
	}
	// サブレンダリング用
	subIndexInfoSolid_ = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);
	subIndexInfoWire_ = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);

	// ポストプロセス用のRootSignatureを生成
	ppRoot_ = std::make_unique<RootSignature>();
	ppRoot_->AddCBVParameter(0, SV_Pixel)	// レンダリング用のデータ
		.AddTableParameter(0, SV_Pixel, {}, 2)	// レンダリングに使うテクスチャ
		//.AddTableParameter(1, SV_Pixel)	// 使用された深度マップを画像として渡す
		.AddSampler(0, SV_Pixel)	// テクスチャのサンプラー
		//.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 深度マップのサンプラー
		.Build(device);
}
void Renderer::SetViewStruct(ViewStruct viewStruct) {
	mainRenderer_->SetViewStruct(viewStruct);
	for (int i = 0; i < kMaxRendering; i++) {
		subRenderer_[i]->SetViewStruct(viewStruct, subIndexInfoSolid_->GetView(), subIndexInfoWire_->GetView());
	}
}
void Renderer::Draw(ID3D12GraphicsCommandList* list) {
	HRESULT hr = S_FALSE;
	
	for (int i = 0; i < subCount_.Get(); i++) {
		subRenderer_[i]->Draw(list, subIndexInfoSolid_->GetCount() / 3, subIndexInfoWire_->GetCount() / 3);
	}
	mainRenderer_->Draw(list);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list->Close();
	assert(SUCCEEDED(hr));
}
void Renderer::Reset() {
	mainRenderer_->Reset();
	subIndexInfoSolid_->Reset();
	subIndexInfoWire_->Reset();
	subCount_.Reset();
}

void Renderer::SetMainRenderTarget(LWP::Object::Camera* camera) {
	mainRenderer_->SetRenderTarget(camera);

	// ポストプロセスをする予定で、ポストプロセス用のコマンドクラスを持っていない場合作成する
	if (camera->isUsePostProcess && !camera->GetPPRenderer()) {
		PostProcessRenderer* newRenderer = new PostProcessRenderer();
		newRenderer->Init(device_, dxc_, heaps_, ppRoot_.get(), camera->shaderPath);
		Math::Vector2 reso = camera->GetRenderTexture()->GetTextureSize();
		newRenderer->SetResolution(static_cast<int>(reso.x), static_cast<int>(reso.y));
		camera->SetPPRenderer(newRenderer);
	}
}
void Renderer::SetSubRenderTarget(LWP::Object::Camera* camera) {
	subRenderer_[subCount_.GetAndIncrement()]->SetRenderTarget(camera);

	// ポストプロセスをする予定で、ポストプロセス用のコマンドクラスを持っていない場合作成する
	if (camera->isUsePostProcess && !camera->GetPPRenderer()) {
		PostProcessRenderer* newRenderer = new PostProcessRenderer();
		newRenderer->Init(device_, dxc_, heaps_, ppRoot_.get(), camera->shaderPath);
		Math::Vector2 reso = camera->GetRenderTexture()->GetTextureSize();
		newRenderer->SetResolution(static_cast<int>(reso.x), static_cast<int>(reso.y));
		camera->SetPPRenderer(newRenderer);
	}
}
void Renderer::AddRenderData(const IndexInfoStruct& indexInfo, const bool& isWireFrame, const bool& isMain) {
	if (isMain) {
		mainRenderer_->AddRenderData(indexInfo, isWireFrame);
	}
	else {
		if (isWireFrame) {
			subIndexInfoWire_->AddData(indexInfo);
		}
		else {
			subIndexInfoSolid_->AddData(indexInfo);
		}
	}
}
void Renderer::AddRenderData2DBill(const IndexInfoStruct& indexInfo) {
	mainRenderer_->AddRenderData2DBill(indexInfo);
}
void Renderer::AddRenderData3DBill(const IndexInfoStruct& indexInfo) {
	mainRenderer_->AddRenderData3DBill(indexInfo);
}
