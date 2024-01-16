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
	index_ = manager->CreateTextureResource(width, height);

	// RTVのインデックス
	//rtvIndex_ = manager->GetRTV()->CreateRenderTargetView();
	// DSVのインデックス
	dsvIndex_ = -1;
}

Vector2 RenderTexture::GetTextureSize() const {
	return { static_cast<float>(kWidth),static_cast<float>(kHeight) };
};