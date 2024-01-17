#include "RenderTexture.h"
#include "../../base/directX/command/CommandManager.h"
#include "../../base/directX/descriptorHeap/RTV.h"
#include "../../base/directX/descriptorHeap/DSV.h"

using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace std;

RenderTexture::RenderTexture(Base::CommandManager* manager, const int width, const int height) 
	: kWidth(width), kHeight(height) {
	resource_ = manager->CreateTextureResource(width, height);

	// テクスチャのインデックス
	srvIndex_ = manager->GetSRV()->CreateShaderResourceView(resource_.Get(), width, height);
	// RTVのインデックス
	rtvIndex_ = manager->GetRTV()->CreateRenderTargetView(resource_.Get());
	// DSVのリソースとインデックス
	dsvIndex_ = manager->GetDSV()->CreateDepthStencil(depthMapResource_.Get(), width, height);

	// テクスチャのみのリソースを生成
	texResource_ = manager->CreateTextureResource(width, height);
	index_ = manager->GetSRV()->CreateShaderResourceView(texResource_.Get(), width, height);
}

Vector2 RenderTexture::GetTextureSize() const {
	return { static_cast<float>(kWidth),static_cast<float>(kHeight) };
};