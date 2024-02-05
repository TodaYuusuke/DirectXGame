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
	renderResource_->RegisterRTV();
	renderResource_->RegisterSRV();
	renderResource_->RegisterDSV();
	// 深度マップをSRVに登録
	renderResource_->RegisterDepthToSRV();

	// テクスチャのみのリソースを生成
	resource_ = directX->GetCommandManager()->CreateTextureResource(width, height);
	index_ = directX->GetHeaps()->srv()->CreateShaderResourceView(resource_.Get(), width, height);	// これはテクスチャ番号
	// SRV上でのインデックスも取得しておく
	srvIndex_ = directX->GetHeaps()->srv()->GetCount() - 1;
}
