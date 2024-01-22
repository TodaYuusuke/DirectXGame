#include "RenderTexture.h"
#include "base/DirectXCommon.h"

using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace std;

RenderTexture::RenderTexture(DirectXCommon* directX, const int width, const int height)
	: kWidth(width), kHeight(height) {
	// レンダーリソース生成
	renderResource_ = std::make_unique<RenderResource>(directX->GetDevice(), directX->GetHeaps(), kWidth, kHeight);
	renderResource_->RegisterSRV();
	renderResource_->RegisterRTV();
	renderResource_->RegisterDSV();

	// テクスチャのみのリソースを生成
	texResource_ = directX->GetCommandManager()->CreateTextureResource(width, height);
	index_ = directX->GetHeaps()->srv()->CreateShaderResourceView(texResource_.Get(), width, height);
}
