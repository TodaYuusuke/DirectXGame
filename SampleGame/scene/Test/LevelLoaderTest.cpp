#include "LevelLoaderTest.h"

// 初期化
void LevelLoaderTest::Initialize() {
	// いったんmainCameraへのデータ適応を行わない
	levelData.SetCameraPtr(nullptr);
	levelData.LoadShortPath("Scene.json");
}

// 更新
void LevelLoaderTest::Update() {
	// RキーでHotReload
	if (lwp::Keyboard::GetTrigger(DIK_R)) {
		levelData.HotReload();
	}
}