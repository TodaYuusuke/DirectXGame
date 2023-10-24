#include "SRV.h"

using namespace LWP::Base;
using namespace Microsoft::WRL;

void SRV::Initialize(ID3D12Device* device) {
	device_ = device;
	size_ = 128;
	// サイズを計算
	kDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, size_, true);
}