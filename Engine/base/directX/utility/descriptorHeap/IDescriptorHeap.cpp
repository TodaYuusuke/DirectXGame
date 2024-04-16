#include "IDescriptorHeap.h"

using namespace LWP::Base;


IDescriptorHeap::IDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size)
	: device_(device),
	kElementSize(device->GetDescriptorHandleIncrementSize(type)),
	kMaxSize(size),
	indexManager_(size) {}


IDescriptorHeap::IDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size, uint32_t size2)
	: IDescriptorHeap(device, type, size + size2) {}

D3D12_CPU_DESCRIPTOR_HANDLE IDescriptorHeap::GetCPUHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = heap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (kElementSize * index);
	return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE IDescriptorHeap::GetGPUHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = heap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (kElementSize * index);
	return handleGPU;
}


ID3D12DescriptorHeap* IDescriptorHeap::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	HRESULT hr = S_FALSE;

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE IDescriptorHeap::GetDescriptorHandleIncrementSize(const D3D12_CPU_DESCRIPTOR_HANDLE& other, int offsetInDescriptors, UINT descriptorIncrementSize) const {
	// ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	if (offsetInDescriptors <= 0) {
		handle = other;
	}
	else {
		handle.ptr = other.ptr + (descriptorIncrementSize * offsetInDescriptors);
	}

	return handle;
}