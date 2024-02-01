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
		subRenderer_[i]->Init(device, mainRenderer_->GetRoot(), mainRenderer_->GetPSO(), heaps);
	}
	// サブレンダリング用
	subIndexInfo_ = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);

	// ポストプロセス用のRootSignatureを生成
	ppRoot_ = std::make_unique<RootSignature>();
	ppRoot_->AddCBVParameter(0, SV_Pixel)	// レンダリング用のデータ
		.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャのサンプラー
		.Build(device);
}
void Renderer::SetViewStruct(ViewStruct viewStruct) {
	mainRenderer_->SetViewStruct(viewStruct);
	for (int i = 0; i < kMaxRendering; i++) {
		subRenderer_[i]->SetViewStruct(viewStruct, subIndexInfo_->GetView());
	}
}
void Renderer::Draw(ID3D12GraphicsCommandList* list) {
	HRESULT hr = S_FALSE;
	
	for (int i = 0; i < subCount_.Get(); i++) {
		subRenderer_[i]->Draw(list, subIndexInfo_->GetCount() / 3);
	}
	mainRenderer_->Draw(list);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list->Close();
	assert(SUCCEEDED(hr));
}
void Renderer::Reset() {
	mainRenderer_->Reset();
	subIndexInfo_->Reset();
	subCount_.Reset();
}

void Renderer::SetMainRenderTarget(LWP::Object::Camera* camera) {
	mainRenderer_->SetRenderTarget(camera);

	// ポストプロセスをする予定で、ポストプロセス用のコマンドクラスを持っていない場合作成する
	if (camera->isUsePostProcess && !camera->GetPPRenderer()) {
		PostProcessRenderer* newRenderer = new PostProcessRenderer();
		newRenderer->Init(device_, dxc_, heaps_, ppRoot_.get(), camera->shaderPath);
		camera->SetPPRenderer(newRenderer);
	}
}
void Renderer::SetSubRenderTarget(LWP::Object::Camera* camera) {
	subRenderer_[subCount_.GetAndIncrement()]->SetRenderTarget(camera);

	// ポストプロセスをする予定で、ポストプロセス用のコマンドクラスを持っていない場合作成する
	if (camera->isUsePostProcess && !camera->GetPPRenderer()) {
		PostProcessRenderer* newRenderer = new PostProcessRenderer();
		newRenderer->Init(device_, dxc_, heaps_, ppRoot_.get(), camera->shaderPath);
		camera->SetPPRenderer(newRenderer);
	}
}
void Renderer::AddMainRenderData(const IndexInfoStruct& indexInfo) {
	mainRenderer_->AddRenderData(indexInfo);
}
void Renderer::AddSubRenderData(const IndexInfoStruct& indexInfo) {
	subIndexInfo_->AddData(indexInfo);
}