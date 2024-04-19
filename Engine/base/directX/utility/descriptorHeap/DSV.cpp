#include "DSV.h"
#include "SRV.h"

#include <Config.h>

using namespace LWP::Base;
using namespace LWP::Utility;
using namespace Microsoft::WRL;

DSV::DSV(ID3D12Device* device) :
	IDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, lwpC::Shadow::kMaxShadowMap + lwpC::Rendering::kMaxMultiWindowRendering + 1) {}

void DSV::Init() {
	// DSV用のヒープでディスクリプタの数はシャドウマップ用などで増加する。DSVはShader内で触らないものなので、ShaderVisibleはfalse
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxSize, false);
}

DSVInfo DSV::CreateDepthStencilView(ID3D12Resource* resource) {
	DSVInfo info;
	// 設定（シェーダーの計算結果をSRGBに変換して書き込む）
	info.desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	info.desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// DSVを作る
	device_->CreateDepthStencilView(resource, &info.desc, info.cpuView);
	return info;
}

DSVInfo DSV::CreateShadowMapDir(ID3D12Resource* resource) {
	DSVInfo info;

	// DSVの設定
	info.desc.Format = DXGI_FORMAT_D32_FLOAT; // Format。基本敵にはResourceに合わせる
	info.desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	info.desc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// DSVに登録
	device_->CreateDepthStencilView(resource, &info.desc, info.cpuView);
	return info;
}

std::array<DSVInfo, 6> DSV::CreateShadowMapPoint(ID3D12Resource* resource) {
	std::array<DSVInfo, 6> info;

	// キューブマップ分生成
	for (int i = 0; i < 6; i++) {
		// DSVの設定
		info[i].desc.Format = DXGI_FORMAT_D32_FLOAT; // Format。基本敵にはResourceに合わせる
		info[i].desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY; // 2DTexture配列
		info[i].desc.Texture2DArray.MipSlice = 0;
		info[i].desc.Texture2DArray.ArraySize = 1;
		info[i].desc.Texture2DArray.FirstArraySlice = i;

		// 空きを使用
		info[i].index = indexManager_.UseEmpty();
		// viewも設定
		info[i].SetView(this);

		// DSVに登録
		device_->CreateDepthStencilView(resource, &info[i].desc, info[i].cpuView);
	}

	return info;
}