#include "Title.h"
#include "GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void Title::Initialize() {
	// 地形初期化
	field_.Init(&levelData, &mainCamera);

	sprite_.material.texture = LWP::Resource::LoadTexture("ui/title.png");
	sprite_.material.enableLighting = false;
	sprite_.isUI = true;
}

// 更新
void Title::Update() {
	// SPACEキーを押すとシーン切り替え
	if (Keyboard::GetTrigger(DIK_SPACE)) {
		nextSceneFunction = []() { return new GameScene(); };
	}
}