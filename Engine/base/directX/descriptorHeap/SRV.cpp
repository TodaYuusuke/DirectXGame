#include "SRV.h"
#include "../Externals/DirectXTex/DirectXTex.h"

using namespace LWP::Base;
using namespace Microsoft::WRL;

void SRV::Initialize(ID3D12Device* device) {
	device_ = device;
	size_ = 128;
	// サイズを計算
	kDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, size_, true);

	// 0番目はImGuiが使うので+1しておく
	count_.Increment();
}

int SRV::CreateShaderResourceView(ID3D12Resource* resource, const DirectX::ScratchImage& mipImages) {
	HRESULT hr = S_FALSE;

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
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	UploadTextureResource(resource, srvDesc);
	return loadedTextureCount++;
}
int SRV::CreateShaderResourceView(ID3D12Resource* resource, const int width, const int height) {
	HRESULT hr = S_FALSE;

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
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	UploadTextureResource(resource, srvDesc);
	return loadedTextureCount++;
}

int SRV::UploadDepthMap(ID3D12Resource* resource) {
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0F;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	
	// SRVを作成するDescriptorHeapの場所を決める
	int srvIndex = GetAndIncrement();
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = GetCPUHandle(srvIndex);
	// SRVの生成
	device_->CreateShaderResourceView(resource, &srvDesc, textureSRVHandleCPU);
	loadedTextureCount++;
	return srvIndex;
}


void SRV::UploadTextureResource(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc) {
	// SRVを作成するDescriptorHeapの場所を決める（ImGuiとStructuredBufferたちが先頭を使っているので注意）
	UINT srvIndex = GetAndIncrement();
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = GetCPUHandle(srvIndex);
	// 初めてのテクスチャ生成ならviewを保存
	if (loadedTextureCount == 0) {
		texView_ = GetGPUHandle(srvIndex);
	}
	// SRVの生成
	device_->CreateShaderResourceView(resource, &desc, textureSRVHandleCPU);
}