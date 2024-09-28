#include "GameScene.h"
#include "Test/Test.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void GameScene::Initialize() {
#if DEMO
#else
	Info::ChangeShowDebugGUI();
#endif
	// 地形初期化
	field_.Init(&levelData, &mainCamera);

	// プレイヤー初期化
	player_.Init(&mainCamera, levelData.terrain.get());
	car_.Init(&mainCamera, levelData.terrain.get());
	drone_.Init(&mainCamera, levelData.terrain.get());
}
// 更新
void GameScene::Update() {
	//player.Update();
	drone_.Update();

	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		levelData.HotReload();
	}
}