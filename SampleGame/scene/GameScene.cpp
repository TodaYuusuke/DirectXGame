#include "GameScene.h"
#include "Test.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void GameScene::Initialize() {
	Info::ChangeShowDebugGUI();

	// モデル用意
	field.Init();
	// プレイヤー初期化
	player.Init(&mainCamera);

	// カメラにポストプロセス用の設定
	mainCamera.pp.CreatePSO("postProcess/PostProcess.PS.hlsl");
}
// 更新
void GameScene::Update() {
	player.Update();

	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextSceneFunction = []() { return new GameScene(); };
	}
	if (Input::Keyboard::GetTrigger(DIK_N)) {
		nextSceneFunction = []() { return new Test(); };
	}
}