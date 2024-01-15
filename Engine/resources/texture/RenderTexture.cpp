#include "RenderTexture.h"
#include "../../base/directX/command/CommandManager.h"

using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace std;

RenderTexture::RenderTexture(Base::CommandManager* manager, const int width, const int height) 
	: kWidth(width), kHeight(height) {
	index_ = manager->CreateTextureResource(width, height);
}

Vector2 RenderTexture::GetTextureSize() const {
	return { static_cast<float>(kWidth),static_cast<float>(kHeight) };
};