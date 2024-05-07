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

	for (int i = 0; i < 10; i++) {
		mesh[i].LoadFile("skydome/skydome.obj");
	}
	// アニメーション用意
	idleAnim[0].LoadAnimationLongPath("resources/model/Player/C_Body.gltf", &mesh[0]);
	idleAnim[1].LoadAnimationLongPath("resources/model/Player/C_Head.gltf", &mesh[0]);
	idleAnim[2].LoadAnimationLongPath("resources/model/Player/C_LHand.gltf", &mesh[0]);
	idleAnim[3].LoadAnimationLongPath("resources/model/Player/C_RHand.gltf", &mesh[0]);
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
}