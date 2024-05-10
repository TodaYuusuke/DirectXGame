#include "Test.h"
//#include "GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void Test::Initialize() {
	// カメラにポストプロセス用の設定
	mainCamera.pp.CreatePSO("postProcess/PostProcess.PS.hlsl");
	subCamera.pp.CreatePSO("postProcess/PostProcess.PS.hlsl");
	// デバッグ情報表示
	Info::ChangeShowDebugGUI();

	mesh.LoadShortPath("human/walk.gltf");

	sprite.texture = subCamera.GetTexture();
}

// 更新
void Test::Update() {
	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextSceneFunction = []() { return new Test(); };
	}
	if (Input::Keyboard::GetTrigger(DIK_N)) {
		//nextSceneFunction = []() { return new GameScene(); };
	}

	// ポストプロセス用のシェーダー更新
	if (Input::Keyboard::GetTrigger(DIK_P)) {
		//mainCamera.pp.CreatePSO();
	}
}