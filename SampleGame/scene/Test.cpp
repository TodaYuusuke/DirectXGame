#include "Test.h"
#include "GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void Test::Initialize() {
	// デバッグ情報表示
	Info::ChangeShowDebugGUI();

	//mesh.LoadShortPath("cube/cube.obj");
	mesh.LoadShortPath("human/walk.gltf");
	cube.LoadCube();
	//anim.LoadAnimationLongPath("resources/model/human/walk.gltf", &mesh);
	//anim.Start();

	sprite.material.texture = subCamera.GetTexture();

	mainCamera.isActive = true;
	mainCamera.pp.use = true;
	mainCamera.pp.vignetting.use = true;

	damageParticle_.model.worldTF.scale = { 0.0001f,0.001f, 0.0001f };
	damageParticle_.model.materials[0].enableLighting = true;
	damageParticle_.model.materials[0].color = Utility::Color(Utility::ColorPattern::RED);
	damageParticle_.isActive = true;

	cube2 = cube;
}

// 更新
void Test::Update() {
	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextSceneFunction = []() { return new Test(); };
	}
	if (Input::Keyboard::GetTrigger(DIK_N)) {
		nextSceneFunction = []() { return new GameScene(); };
	}

	// ポストプロセス用のシェーダー更新
	if (Input::Keyboard::GetTrigger(DIK_P)) {
		damageParticle_.Add(32);
	}
}