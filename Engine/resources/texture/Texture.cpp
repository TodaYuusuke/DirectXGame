#include "Texture.h"
#include "../../base/DirectXCommon.h"
#include "../../utility/MyUtility.h"

using namespace LWP::Base;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace std;

Texture::Texture(DirectXCommon* directX, const std::string& filePath) {
	Load(filePath);
	textureResource = directX->CreateTextureResource(mipImages.GetMetadata());
	index = directX->UploadTextureData(textureResource, &textureSRVHandleGPU, mipImages);
}


void Texture::Load(const string& filePath) {
	// テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
}