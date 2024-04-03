#include "HeapManager.h"

using namespace LWP::Base;

HeapManager::HeapManager(HWND hwnd, GPUDevice* gpuDevice, int32_t width, int32_t height, ID3D12CommandQueue* queue) {
	
	// RTV作成
	rtv_ = std::make_unique<RTV>();
	rtv_->Initialize(hwnd, gpuDevice, width, height, queue);
	// SRV作成
	srv_ = std::make_unique<SRV>();
	srv_->Initialize(gpuDevice->GetDevice());
	// DSV作成
	dsv_ = std::make_unique<DSV>();
	dsv_->Initialize(gpuDevice->GetDevice(), srv_.get());
}
