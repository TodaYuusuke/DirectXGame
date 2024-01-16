#include "Texture.h"
#include "../../base/DirectXCommon.h"
#include "../../base/directX/command/CommandManager.h"
#include "../../utility/MyUtility.h"

using namespace LWP::Base;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace std;

Texture::Texture(Base::CommandManager* manager, const std::string& filePath) {
	Load(filePath);
	resource_ = manager->CreateTextureResource(mipImages.GetMetadata());

	// テクスチャのインデックス
	index_ = manager->GetSRV()->CreateShaderResourceView(resource_.Get(), mipImages);
}

LWP::Math::Vector2 Texture::GetTextureSize() const {
	return LWP::Math::Vector2{ static_cast<float>(mipImages.GetImages()->width),static_cast<float>(mipImages.GetImages()->height) };
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