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
	//Info::ChangeShowDebugGUI();
	levelData.LoadShortPath("SampleGameScene.json");
	
	// bloomをON
	/*mainCamera.pp.use = true;
	mainCamera.pp.outLine.use = true;
	mainCamera.pp.bloom.use = true;
	mainCamera.pp.CreateShaderFile();*/

	skydome.LoadShortPath("skydome/skydome.obj");
	skydome.worldTF.scale = { 100.0f,100.0f,100.0f };
	skydome.materials[1].color = Color(5, 5, 16, 255);
	skydome.SetAllMaterialLighting(false);
	for (int i = 0; i < kStarCount; i++) {
		stars[i].LoadCube();
		stars[i].worldTF.translation = Vector3{
			LWP::Utility::GenerateRandamNum<int>(-100, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(-100, 100) / 100.0f,
		}.Normalize() * 99.0f;
		stars[i].worldTF.rotation = Quaternion{
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			1.0f
		}.Normalize();
		float scale = LWP::Utility::GenerateRandamNum<int>(10, 30) / 100.0f;
		stars[i].worldTF.scale = { scale,scale,scale };

		stars[i].materials[0].color = Utility::ColorPattern::YELLOW;
		stars[i].SetAllMaterialLighting(false);
	}
	ground.LoadShortPath("ground/Ground.gltf");
	ground.ApplyWorldTransform({
		{},
		Quaternion(),
		{ 100.0f,1.0f, 100.0f }
	});
	Collider::Mesh& colliderMesh = groundCollider.SetBroadShape(Collider::Mesh());
	colliderMesh.Create(ground);
	//ground.SetAllMaterialLighting(true);

	// プレイヤー初期化
	player.Init(&mainCamera);
}
// 更新
void GameScene::Update() {
	player.Update();

	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		levelData.HotReload();
	}
	//if (Input::Keyboard::GetTrigger(DIK_N)) {
	//	nextSceneFunction = []() { return new Test(); };
	//}
}