#include "RTV.h"

#include <Config.h>

using namespace LWP::Utility;
using namespace LWP::Base;
using namespace Microsoft::WRL;

RTV::RTV(ID3D12Device* device) :
	IDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, lwpC::Rendering::kMaxMultiWindowRendering + 2) {}

void RTV::Init() {
	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxSize, false);
}

RTVInfo RTV::CreateRenderTargetView(ID3D12Resource* resource) {
	RTVInfo info;
	// 設定（シェーダーの計算結果をSRGBに変換して書き込む）
	info.desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	info.desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	device_->CreateRenderTargetView(resource, &info.desc, info.cpuView);
	return info;
}
