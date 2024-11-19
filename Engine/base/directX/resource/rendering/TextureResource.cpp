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
	properties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルト

	// 3. Resourceを生成する
	hr = device->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	// データ転送される設定
		nullptr,				// 使わないのでnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// 4. VRAM配置用の一時リソースも作成
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(device->GetDevice(), mipImages_.GetImages(), mipImages_.GetImageCount(), mipImages_.GetMetadata(), subResources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(resource_.Get(), 0, UINT(subResources.size()));
	intermediateResource_ = CreateResource(device, intermediateSize);

	// VRAMにアップロード
	info_ = heaps->srv()->CreateTexture(resource_.Get(), intermediateResource_, &subResources, mipImages_);
}


void TextureResource::Load(const std::string& filePath) {
	HRESULT hr;
	// テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = Utility::ConvertString(filePath);
	
	// 拡張子によって読み込み方を変える
	if (Utility::GetExtension(filePathW) == L"dds") {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, mipImages_);
		assert(SUCCEEDED(hr));
		// ミップマップは生成済みと仮定
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
		// ミップマップの作成
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages_);
		assert(SUCCEEDED(hr));
	}
}

ID3D12Resource* TextureResource::CreateResource(GPUDevice* device, size_t size) {
	HRESULT hr = S_FALSE;

	// ヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
	// 設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = size; // リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ID3D12Resource* resource;
	hr = device->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}
