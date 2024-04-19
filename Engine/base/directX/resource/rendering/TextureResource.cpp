#include "TextureResource.h"
#include "../Externals/DirectXTex/DirectXTex.h"
#include "../Externals/DirectXTex/d3dx12.h"

#include "utility/MyUtility.h"

using namespace LWP;
using namespace LWP::Base;
	
void TextureResource::Init(GPUDevice* device, HeapManager* heaps, std::string filepath) {
	HRESULT hr = S_FALSE;
	// ファイル読み込み
	Load(filepath);
	// メタデータ読み込み
	const DirectX::TexMetadata metaData(mipImages_.GetMetadata());
	// 解像度保持
	width = static_cast<int>(metaData.width);
	height = static_cast<int>(metaData.height);

	// 1. Resourceの設定
	desc.Width = UINT(metaData.width);
	desc.Height = UINT(metaData.height);
	desc.MipLevels = UINT16(metaData.mipLevels);
	desc.DepthOrArraySize = UINT16(metaData.arraySize);
	desc.Format = metaData.format;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);

	// 2. 利用するHeapの設定。非常に特殊な運用。
	properties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// 3. Resourceを生成する
	hr = device->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 初回のResourceState。読み取り専用
		nullptr,				// 使わないのでnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// テクスチャのインデックス
	info_ = heaps->srv()->CreateTexture(resource_.Get(), mipImages_);
}


void TextureResource::Load(const std::string& filePath) {
	// テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = Utility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages_);
	assert(SUCCEEDED(hr));
}

//TextureResource::operator Resource::Texture() { return *this; }