#include "SRV.h"
#include "base/directX/resource/data/IDataResource.h"

#include "../Externals/DirectXTex/DirectXTex.h"
#include "../Externals/DirectXTex/d3dx12.h"

#include "base/ImGuiManager.h"

using namespace LWP::Base;
using namespace Microsoft::WRL;

// コンフィグデータの短縮形
#define cBuf lwpC::Rendering::kMaxBuffer			// バッファーの数
#define cTex lwpC::Rendering::kMaxTexture			// テクスチャの数
#define cTexOffset cBuf
#define cDirS lwpC::Shadow::Direction::kMaxCount	// 平行光源の数
#define cDirSOffset cBuf+cTex
#define cPointS lwpC::Shadow::Point::kMaxCount		// 点光源の数
#define cPointSOffset cBuf+cTex+cDirS

SRV::SRV() :
	IDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cBuf + cTex + cDirS + cPointS, cBuf),
	textureIM_(cTex, cTexOffset),
	dirShadowMapIM_(cDirS, cDirSOffset),
	pointShadowIM_(cPointS, cPointSOffset) {
	// SRV用のヒープでディスクリプタの数は1000。SRVはShader内で触るものなので、ShaderVisibleはtrue
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSize, true);
}

void SRV::DebugGUI() {
	if (ImGui::CollapsingHeader("SRV")) {
		if (ImGui::TreeNode("StructerdBuffer Index")) {
			indexManager_.DebugGUI();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Texture Index")) {
			textureIM_.DebugGUI();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("DirShadowMap Index")) {
			dirShadowMapIM_.DebugGUI();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("PointShadowMap Index")) {
			pointShadowIM_.DebugGUI();
			ImGui::TreePop();
		}
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE SRV::GetFirstTexView() { return GetGPUHandle(cTexOffset); }
D3D12_GPU_DESCRIPTOR_HANDLE SRV::GetFirstDirShadowView() { return GetGPUHandle(cDirSOffset); }
D3D12_GPU_DESCRIPTOR_HANDLE SRV::GetFirstPointShadowView() { return GetGPUHandle(cPointSOffset); }


SRVInfo SRV::CreateTexture(ID3D12Resource* resource, ID3D12Resource* intermediateResource, std::vector<D3D12_SUBRESOURCE_DATA>* subResources, const DirectX::ScratchImage& mipImages) {
	SRVInfo info;

	UpdateSubresources(cmd_->List(), resource, intermediateResource, 0, 0, UINT(subResources->size()), subResources->data());
	cmd_->SetReleaseFunction([intermediateResource]() { intermediateResource->Release(); });
	// テクスチャへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	cmd_->List()->ResourceBarrier(1, &barrier);
	
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	// metaDataを元にSRVの設定
	info.desc.Format = metadata.format;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	info.desc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// 空きを使用
	info.index = textureIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに生成
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}
SRVInfo SRV::CreateDepthTexture(ID3D12Resource* resource) {
	SRVInfo info;

	// SRVの設定
	info.desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	info.desc.Texture2D.MipLevels = 1;
	//info.desc.Texture2D.MostDetailedMip = 0;
	//info.desc.Texture2D.PlaneSlice = 0;
	//info.desc.Texture2D.ResourceMinLODClamp = 0.0F;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 空きを使用
	info.index = textureIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}
UAVInfo SRV::CreateStencilUAV(ID3D12Resource* resource) {
	UAVInfo info;

	// SRVの設定
	info.desc.Format = DXGI_FORMAT_R8_UINT;
	info.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	// 空きを使用
	info.index = textureIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	GPUDevice::GetInstance()->GetDevice()->CreateUnorderedAccessView(resource, nullptr, &info.desc, info.cpuView);
	return info;
}
SRVInfo SRV::CreateRenderResource(ID3D12Resource* resource) {
	SRVInfo info;

	// SRVの設定
	info.desc.Format = resource->GetDesc().Format;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	info.desc.Texture2D.MipLevels = 1;

	// 空きを使用
	info.index = textureIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに生成
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}
UAVInfo SRV::CreateRenderResourceUAV(ID3D12Resource* resource) {
	UAVInfo info;

	// UAVの設定
	info.desc.Format = resource->GetDesc().Format;
	info.desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	// 空きを使用
	info.index = textureIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに生成
	GPUDevice::GetInstance()->GetDevice()->CreateUnorderedAccessView(resource, nullptr, &info.desc, info.cpuView);
	return info;
}
SRVInfo SRV::CreateCubeMap(ID3D12Resource* resource) {
	// SRVにテクスチャとして登録（キューブマップ）
	SRVInfo info;	// SRVの登録情報

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	info.desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	info.desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	info.desc.TextureCube.MipLevels = UINT_MAX;
	info.desc.TextureCube.MostDetailedMip = 0;
	info.desc.TextureCube.ResourceMinLODClamp = 0.0F;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 空きを使用
	info.index = textureIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
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
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}
UAVInfo SRV::CreateRWStructuredBuffer(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc) {
	// SRVの登録情報
	UAVInfo info;

	// 設定をコピー
	info.desc = desc;
	// 空きを使用
	info.index = indexManager_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	GPUDevice::GetInstance()->GetDevice()->CreateUnorderedAccessView(resource, nullptr, &info.desc, info.cpuView);
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
	info.desc.Texture2D.ResourceMinLODClamp = 0.0f;
	info.desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 空きを使用
	info.index = dirShadowMapIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
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
	info.index = pointShadowIM_.UseEmpty();
	// viewも設定
	info.SetView(this);

	// SRVに登録
	GPUDevice::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &info.desc, info.cpuView);
	return info;
}

SRVInfo SRV::CreateImGuiSpace() {
	SRVInfo info;
	// インデックスを登録
	info.index = indexManager_.UseEmpty();
	info.SetView(this);
	return info;
}