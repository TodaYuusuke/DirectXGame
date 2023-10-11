#include "GameScene.h"

using namespace LWP::Primitive;

// 初期化
void GameScene::Initialize() {
	// カメラを上から見下ろす形に
	mainCamera->transform.translation = { 0.0f,3.83f,0.0f };
	mainCamera->transform.rotation = { 1.57f,0.0f,0.0f };

	mapManager_.Initialize();
}
// 更新
void GameScene::Update() {
	mapManager_.Update();

	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
}
// 描画
void GameScene::Draw() {
}