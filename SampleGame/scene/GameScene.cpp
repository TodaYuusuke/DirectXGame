#include "GameScene.h"
#include "Test/Test.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void GameScene::Initialize() {
	LWP::Info::ChangeShowDebugGUI();

	buildings[0].LoadShortPath("buildings/1Story_Mat.gltf");
	buildings[1].LoadShortPath("buildings/1Story_GableRoof_Mat.gltf");
	buildings[2].LoadShortPath("buildings/1Story_RoundRoof_Mat.gltf");
	buildings[3].LoadShortPath("buildings/2Story_Mat.gltf");
	buildings[4].LoadShortPath("buildings/2Story_Sign_Mat.gltf");
	buildings[5].LoadShortPath("buildings/3Story_Balcony_Mat.gltf");
	buildings[6].LoadShortPath("buildings/4Story_Center_Mat.gltf");
	buildings[7].LoadShortPath("buildings/4Story_Wide_2Doors_Mat.gltf");
	for (int i = 0; i < 8; i++) {
		buildings[i].worldTF.translation.x += i * 2.0f;
		buildings[i].worldTF.translation.z = 2.0f;
		//buildings[i].worldTF.rotation.y = 3.14f;
		buildings[i].enableLighting = true;
	}
	buildings[6].worldTF.translation.x = 3.0f;
	buildings[6].worldTF.translation.z = 7.0f;
	buildings[7].worldTF.translation.x = -8.0f;
	buildings[7].worldTF.translation.z = 7.0f;


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