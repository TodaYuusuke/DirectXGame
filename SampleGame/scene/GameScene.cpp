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
	levelData.LoadShortPath("SampleGameScene.json");
	
	// 複数画面描画確認
	//sprite_.material.texture = subCamera.GetTexture();
	//sprite_.material.enableLighting = false;
	//sprite_.isActive = true;
	//sprite_.name = "Sprite";

	// bloomをON
	/*mainCamera.pp.use = true;
	mainCamera.pp.outLine.use = true;
	mainCamera.pp.bloom.use = true;
	mainCamera.pp.CreateShaderFile();*/

	const float s = 1000.0f;
	skydome.LoadShortPath("skydome/skydome.obj");
	skydome.worldTF.scale = { s,s,s };
	//skydome.materials[1].color = Color(5, 5, 16, 255);
	skydome.SetAllMaterialLighting(false);
	for (int i = 0; i < kStarCount; i++) {
		stars[i].LoadCube();
		stars[i].worldTF.translation = Vector3{
			LWP::Utility::GenerateRandamNum<int>(-100, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(-100, 100) / 100.0f,
		}.Normalize() * (s - 1.0f);
		stars[i].worldTF.rotation = Quaternion{
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			1.0f
		}.Normalize();
		float scale = LWP::Utility::GenerateRandamNum<int>(50, 100) / 100.0f;
		stars[i].worldTF.scale = { scale,scale,scale };

		//stars[i].materials[0].color = Utility::ColorPattern::YELLOW;
		stars[i].SetAllMaterialLighting(false);
	}

	//sun.rotation.x = 0.0f;

	// プレイヤー初期化
	player.Init(&mainCamera, levelData.terrain.get());
	car.Init(&mainCamera, levelData.terrain.get());
}
// 更新
void GameScene::Update() {
	if (isDriving) {
		car.Update();
	}
	else {
		player.Update();
		//car..worldTF.rotation *= Quaternion::CreateFromAxisAngle(Vector3::UnitY(), 0.02f);
	}

	// 車乗り降り
	if (Input::Keyboard::GetTrigger(DIK_E)) {
		isDriving = !isDriving;
	}

	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		levelData.HotReload();
	}
	//if (Input::Keyboard::GetTrigger(DIK_N)) {
	//	nextSceneFunction = []() { return new Test(); };
	//}
}