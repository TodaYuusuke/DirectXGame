#include "RenderTexture.h"
#include "../../base/DirectXCommon.h"

using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace std;

RenderTexture::RenderTexture(Base::DirectXCommon* directX, const int width, const int height)
	: kWidth(width), kHeight(height) {
	resource_ = directX->GetCommandManager()->CreateTextureResource(width, height);

	// テクスチャのインデックス
	srvIndex_ = directX->GetHeaps()->srv()->CreateShaderResourceView(resource_.Get(), width, height);
	// RTVのインデックス
	rtvIndex_ = directX->GetHeaps()->rtv()->CreateRenderTargetView(resource_.Get());
	// DSVのリソースとインデックス
	dsvIndex_ = directX->GetHeaps()->dsv()->CreateDepthStencil(depthMapResource_.Get(), width, height);

	// テクスチャのみのリソースを生成
	texResource_ = directX->GetCommandManager()->CreateTextureResource(width, height);
	index_ = directX->GetHeaps()->srv()->CreateShaderResourceView(texResource_.Get(), width, height);
}

Vector2 RenderTexture::GetTextureSize() const {
	return { static_cast<float>(kWidth),static_cast<float>(kHeight) };
};