#include "SRV.h"
#include "../Externals/DirectXTex/DirectXTex.h"
#include "../Externals/DirectXTex/d3dx12.h"

using namespace LWP::Base;
using namespace Microsoft::WRL;

SRV::SRV(ID3D12Device* device) :
	IDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 500, 500), texIndexManager_(500, 500) {}

void SRV::Init() {
	// SRV用のヒープでディスクリプタの数は1000。SRVはShader内で触るものなので、ShaderVisibleはtrue
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSize, true);
	
	// 0番目はImGuiが使うので+1しておく
	//count_.Increment();
	//indexManager_.UseEmpty();
}

SRVInfo SRV::CreateTexture(ID3D12Resource* resource, const DirectX::ScratchImage& mipImages) {
	HRESULT hr = S_FALSE;
	SRVInfo info;

	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		hr = resource->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}

	// metaDataを元にSRVの設定
	info.desc.Format = metadata.format;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	info.desc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに生成
	device_->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}
SRVInfo SRV::CreateRenderResource(ID3D12Resource* resource, const int width, const int height) {
	HRESULT hr = S_FALSE;
	SRVInfo info;

	// 画素数
	const UINT pixelCount = width * height;
	// 画像1行分のデータサイズ
	const UINT rowPitch = sizeof(UINT) * width;
	// 画像全体のデータサイズ
	const UINT depthPitch = rowPitch * height;
	// 画像イメージ
	UINT* img = new UINT[pixelCount];
	// 一度緑で画像を初期化
	for (UINT i = 0; i < pixelCount; i++) { img[i] = 0xFF00FF00; }

	// TextureBufferに転送
	hr = resource->WriteToSubresource(
		0, nullptr, img, rowPitch, depthPitch
	);
	assert(SUCCEEDED(hr));

	// SRVの設定
	info.desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	info.desc.Texture2D.MipLevels = 1;

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに生成
	device_->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}

SRVInfo SRV::CreateStructuredBuffer(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
	// SRVの登録情報
	SRVInfo info;

	// 設定をコピー
	info.desc = desc;
	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	device_->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}


SRVInfo SRV::CreateShadowMapDir(ID3D12Resource* resource) {
	// SRVにテクスチャとして登録
	SRVInfo info;	// SRVの登録情報

	// 設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	info.desc.Format = DXGI_FORMAT_R32_FLOAT;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	info.desc.Texture2D.MipLevels = 1;
	info.desc.Texture2D.MostDetailedMip = 0;
	info.desc.Texture2D.PlaneSlice = 0;
	info.desc.Texture2D.ResourceMinLODClamp = 0.0F;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	device_->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}

SRVInfo SRV::CreateShadowMapPoint(ID3D12Resource* resource) {
	// SRVにテクスチャとして登録（キューブマップ）
	SRVInfo info;	// SRVの登録情報

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	info.desc.Format = DXGI_FORMAT_R32_FLOAT;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	info.desc.TextureCube.MipLevels = 1;
	info.desc.TextureCube.MostDetailedMip = 0;
	info.desc.TextureCube.ResourceMinLODClamp = 0.0F;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	device_->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}


//int SRV::UploadDepthMap(ID3D12Resource* resource) {
//	// SRVの設定
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = 1;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	srvDesc.Texture2D.PlaneSlice = 0;
//	srvDesc.Texture2D.ResourceMinLODClamp = 0.0F;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	
//	// SRVを作成するDescriptorHeapの場所を決める
//	int srvIndex = GetAndIncrement();
//	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = GetCPUHandle(srvIndex);
//	// SRVの生成
//	device_->CreateShaderResourceView(resource, &srvDesc, textureSRVHandleCPU);
//
//	loadedTextureCount++;
//	return srvIndex;
//}

SRVInfo SRV::CreateImGuiSpace() {
	SRVInfo info;
	// インデックスを登録
	info.index = texIndexManager_.UseEmpty();
	info.SetView(this);
	return info;
}

