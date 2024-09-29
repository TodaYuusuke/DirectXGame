#include "NullScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

NullScene::NullScene(std::function<IScene*()> func) {
	sprite_.worldTF.scale.x = 10.0f;
	sprite_.worldTF.scale.y = 6.0f;
	sprite_.isUI = true;
	sprite_.material.enableLighting = false;
	sprite_.material.color = ColorPattern::BLACK;
	nextSceneFunction = func;
}
