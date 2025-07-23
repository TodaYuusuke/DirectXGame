#include "NoneScene.h"

// 初期化
void NoneScene::Initialize() {
	axis.LoadShortPath("axis.obj");
	test.LoadTexture("uvChecker.dds");
	test.anchorPoint = { 0.0f, 0.5f };
	test.worldTF.scale.y = 0.3f;
}

// 更新
void NoneScene::Update() {}