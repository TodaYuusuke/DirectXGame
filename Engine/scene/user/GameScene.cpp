#include "GameScene.h"

using namespace LWP::Primitive;

// 初期化
void GameScene::Initialize() {
	tri = LWP::Primitive::CreateInstance<Triangle>();

	tri->vertices[0] = { 0.0f,0.2f,0.0f };
	tri->vertices[1] = { 0.2f,0.0f,0.0f };
	tri->vertices[2] = { -0.2f,0.0f,0.0f };

	target_ = new LWP::Object::WorldTransform;
	target_->Initialize();

	enemy = new Enemy(target_);
}
// 更新
void GameScene::Update() {
	enemy->Update();
}
// 描画
void GameScene::Draw() {
	//tri->Draw();
	enemy->Draw();
}