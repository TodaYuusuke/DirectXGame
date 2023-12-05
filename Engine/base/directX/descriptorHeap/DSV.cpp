#include "DSV.h"
#include "SRV.h"

#include <Config.h>
namespace RenderingPara = LWP::Config::Rendering;

using namespace LWP::Base;
using namespace Microsoft::WRL;

void DSV::Initialize(ID3D12Device* device, SRV* srv) {
	device_ = device;
	srv_ = srv;
	size_ = RenderingPara::kMaxShadowMap + 1;
	// サイズを計算
	kDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// DSV用のヒープでディスクリプタの数は2。DSVはShader内で触らないものなので、ShaderVisibleはfalse
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, size_, false);
}

void DSV::ClearDepth(UINT index, ID3D12GraphicsCommandList* commandList) {
	// 指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(GetCPUHandle(index), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

uint32_t DSV::CreateDepthStencil(ID3D12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE* view, int32_t width, int32_t height) {
	HRESULT hr = S_FALSE;

	// DSVResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width; // Textureの幅
	resourceDesc.Height = height; // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度地のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Resourceの生成
	hr = device_->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&resource) // 作成するリソースへのポインタ
	);
	assert(SUCCEEDED(hr));

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本敵にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(resource, &dsvDesc, GetCPUHandle(GetCount()));
	*view = GetCPUHandle(GetCount());
	return GetAndIncrement();
}

ID3D12Resource* DSV::CreateDirectionShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view) {
	HRESULT hr = S_FALSE;

	ID3D12Resource* resource;

	// DSVResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = 1024 * static_cast<int>(RenderingPara::kShadowMapResolutionScale); // Textureの幅
	resourceDesc.Height = 1024 * static_cast<int>(RenderingPara::kShadowMapResolutionScale); // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度地のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.DepthStencil.Stencil = 0;

	// Resourceの生成
	hr = device_->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, // デフォルトは読み取り専用
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&resource) // 作成するリソースへのポインタ
	);
	assert(SUCCEEDED(hr));

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Format。基本敵にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(resource, &dsvDesc, GetCPUHandle(GetCount()));

	// SRVにテクスチャとして登録

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0F;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


	// SRVを作成するDescriptorHeapの場所を決める（ImGuiが先頭を使っているので+1）
	int srvIndex = srv_->GetAndIncrement();
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = srv_->GetCPUHandle(srvIndex);
	*view = srv_->GetGPUHandle(srvIndex);
	// SRVの生成
	device_->CreateShaderResourceView(resource, &srvDesc, textureSRVHandleCPU);
	*dsvIndex = GetAndIncrement();
	return resource;
}

ID3D12Resource* DSV::CreatePointShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view) {
	HRESULT hr = S_FALSE;

	ID3D12Resource* resource;
	
	// DSVResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = 1024 * static_cast<int>(RenderingPara::kShadowMapResolutionScale); // Textureの幅
	resourceDesc.Height = 1024 * static_cast<int>(RenderingPara::kShadowMapResolutionScale); // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 6; // キューブマップなので6
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度地のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.DepthStencil.Stencil = 0;

	// Resourceの生成
	hr = device_->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, // デフォルトは読み取り専用
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&resource) // 作成するリソースへのポインタ
	);
	assert(SUCCEEDED(hr));


	for (int i = 0; i < 6; i++) {
		// DSVの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Format。基本敵にはResourceに合わせる
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY; // 2DTexture配列
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.ArraySize = 1;
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		// DSVを作る
		device_->CreateDepthStencilView(resource, &dsvDesc, GetCPUHandle(GetCount()));
		dsvIndex[i] = GetAndIncrement();
	}

	// SRVにテクスチャとして登録

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0F;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = srv_->GetCPUHandle(srv_->GetCount());
	*view = srv_->GetGPUHandle(srv_->GetAndIncrement());
	// SRVの生成
	device_->CreateShaderResourceView(resource, &srvDesc, textureSRVHandleCPU);

	return resource;
}