#include "SampleGameScene.h"
#include "SampleTitle.h"

using namespace LWP;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void SampleGameScene::Initialize() {
	// カメラを上から見下ろす形に
	mainCamera->transform.translation = { 0.0f,4.5f,0.0f };
	mainCamera->transform.rotation = { 1.57f,0.0f,0.0f };
	// 前から見たカメラ
	//mainCamera->transform.translation = { 0.0f,3.5f,-3.0f };
	//mainCamera->transform.rotation = { 0.85f,0.0f,0.0f };
	
	tambourine_.Initialize();
	skydome_.Initialze();
	mapManager_.Initialize();
	hammer_.Initialize(&mapManager_);
	mobManager_.Initialize();
}
// 更新
void SampleGameScene::Update() {
	mapManager_.Update();
	hammer_.Update();
	mobManager_.Update(&mapManager_);

#if _DEBUG
	ImGui::Begin("GameScene");
	ImGui::Text("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
#endif

	// Cキーを押すと敵召喚
	if (Input::Keyboard::GetTrigger(DIK_C)) {
		mobManager_.AddEnemy(1);
	}

	// Rキーを押すとシーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextScene_ = new SampleGameScene();
	}
	// ENTERキーを押すとシーン切り替え
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new SampleTitle();
	}
}