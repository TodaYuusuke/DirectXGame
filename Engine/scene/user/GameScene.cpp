#include "GameScene.h"
#include "Title.h"

using namespace LWP::Primitive;

// 初期化
void GameScene::Initialize() {
	// カメラを上から見下ろす形に
	//mainCamera->transform.translation = { 0.0f,4.5f,0.0f };
	//mainCamera->transform.rotation = { 1.57f,0.0f,0.0f };
	// 前から見たカメラ
	mainCamera->transform.translation = { 0.0f,3.5f,-3.0f };
	mainCamera->transform.rotation = { 0.85f,0.0f,0.0f };


	mapManager_.Initialize();
	hammer_.Initialize(&mapManager_);
}
// 更新
void GameScene::Update() {
	mapManager_.Update();
	hammer_.Update();

	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();


	// ENTERキーを押すとシーン切り替え
	if (LWP::Input::GetTrigger(DIK_RETURN)) {
		nextScene_ = new Title();
	}
}
// 描画
void GameScene::Draw() {
}