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

	mesh.LoadFile("skydome/skydome.obj");
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

	if (Input::Keyboard::GetTrigger(DIK_A)) {
		indexes.push_back(indexManager.UseEmpty());
	}
	if (Input::Keyboard::GetTrigger(DIK_D)) {
		indexes.pop_back();
	}

	ImGui::Begin("Test");
	indexManager.DebugGUI();
	ImGui::End();
}