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
	buildings[0].LoadShortPath("buildings/1Story.obj");
	buildings[1].LoadShortPath("buildings/1Story_GableRoof.obj");
	buildings[2].LoadShortPath("buildings/1Story_RoundRoof.obj");
	buildings[3].LoadShortPath("buildings/1Story_Sign.obj");
	buildings[4].LoadShortPath("buildings/2Story.obj");
	buildings[5].LoadShortPath("buildings/2Story_Center.obj");
	for (int i = 0; i < 6; i++) {
		buildings[i].worldTF.translation.x += i * 2.0f;
		buildings[i].worldTF.translation.z = 2.0f;
		buildings[i].worldTF.rotation *= Quaternion::ConvertEuler({ 0.0f,3.14f,0.0f });
		buildings[i].enableLighting = true;
	}

	skydome.LoadShortPath("skydome/skydome.obj");
	skydome.worldTF.scale = { 100.0f,100.0f, 100.0f };
	ground.LoadShortPath("ground/Ground.gltf");
	ground.worldTF.scale = { 100.0f,1.0f, 100.0f };
	ground.enableLighting = true;

	// プレイヤー初期化
	player.Init(&mainCamera);
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