#include "GameScene.h"

using namespace LWP::Primitive;

// 初期化
void GameScene::Initialize() {
	tri = LWP::Engine::CreatePrimitiveInstance<Triangle>();

	tri->vertices[0] = { 0.0f,0.2f,0.0f };
	tri->vertices[1] = { 0.2f,0.0f,0.0f };
	tri->vertices[2] = { -0.2f,0.0f,0.0f };
}
// 更新
void GameScene::Update() {

}
// 描画
void GameScene::Draw() {
	tri->Draw();
}