#include "PostProcessManager.h"

using namespace LWP::Base::PostProcess;

void Manager::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps) {
	// リソースを生成
	lensDistortion_ = std::make_unique<LensDistortionRenderer>();
	lensDistortion_->Init(device, dxc, heaps);
	cctvEffect_ = std::make_unique<CCTVEffectRenderer>();
	cctvEffect_->Init(device, dxc, heaps);
}
void Manager::Draw(ID3D12GraphicsCommandList* list) {
	lensDistortion_->Draw(list);
	cctvEffect_->Draw(list);
}
void Manager::Reset() {
	lensDistortion_->Reset();
	cctvEffect_->Reset();
}

void Manager::SetRenderData(Resource::RenderTexture* target, CCTVEffect data) {
	cctvEffect_->SetRenderData(target, data);
}
void Manager::SetRenderData(Resource::RenderTexture* target, LensDistortion data) {
	lensDistortion_->SetRenderData(target, data);
}