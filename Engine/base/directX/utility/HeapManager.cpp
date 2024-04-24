#include "HeapManager.h"

using namespace LWP::Base;

HeapManager::HeapManager(GPUDevice* gpuDevice) {
	
	// RTV作成
	rtv_ = std::make_unique<RTV>(gpuDevice->GetDevice());
	rtv_->Init();
	// SRV作成
	srv_ = std::make_unique<SRV>(gpuDevice->GetDevice());
	srv_->Init();
	// DSV作成
	dsv_ = std::make_unique<DSV>(gpuDevice->GetDevice());
	dsv_->Init();
}
