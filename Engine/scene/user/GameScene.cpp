#include "GameScene.h"
#include "Title.h"

using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void GameScene::Initialize() {
	// カメラを上から見下ろす形に
	mainCamera->transform.translation = { 0.0f,4.5f,0.0f };
	mainCamera->transform.rotation = { 1.57f,0.0f,0.0f };
	// 前から見たカメラ
	//mainCamera->transform.translation = { 0.0f,3.5f,-3.0f };
	//mainCamera->transform.rotation = { 0.85f,0.0f,0.0f };
	

	mapManager_.Initialize();
	hammer_.Initialize(&mapManager_);
	mobManager_.Initialize();
}
// 更新
void GameScene::Update() {
	mapManager_.Update();
	hammer_.Update();
	mobManager_.Update(&mapManager_);

	ImGui::Begin("GameScene");
	ImGui::Text("Camera");
	mainCamera->DebugGUI();
	ImGui::End();

	// Cキーを押すと敵召喚
	if (LWP::Input::GetTrigger(DIK_C)) {
		mobManager_.AddEnemy(1);
	}

	// Rキーを押すとシーン再読み込み
	if (LWP::Input::GetTrigger(DIK_R)) {
		nextScene_ = new GameScene();
	}
}
// 描画
void GameScene::Draw() {}