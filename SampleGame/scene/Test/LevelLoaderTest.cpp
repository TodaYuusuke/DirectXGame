#include "LevelLoaderTest.h"

using namespace LWP::Math;

// 初期化
void LevelLoaderTest::Initialize() {
	levelData.LoadShortPath("Scene.json");
	//levelData.rigidModels["Cube"].enableLighting = true;
}

// 更新
void LevelLoaderTest::Update() {
	// RキーでHotReload
	if (lwp::Keyboard::GetTrigger(DIK_R)) {
		levelData.HotReload();
	}
	// Tキーで回転
	if (lwp::Keyboard::GetTrigger(DIK_T)) {
		//levelData.rigidModels["Cube"].worldTF.rotation =
		//	Quaternion::CreateFromAxisAngle({1.0f,0.0f,0.0f}, -static_cast<float>(M_PI) / 2.0f) * levelData.rigidModels["Cube"].worldTF.rotation;
	}
}