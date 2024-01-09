#include "GamePlay.h"
#include "Result.h"

void GamePlay::Initialize() {
	// スカイドームの生成
	skydome_ = LWP::Resource::LoadModel("skydome/skydome.obj");
	skydome_->transform.scale = { 500, 500, 500 };
	// グラウンドの生成
	ground_ = LWP::Resource::LoadModel("ground/ground.obj");
	ground_->transform.scale = { 50, 50, 50 };
	ground_->material.enableLighting = true;
	// 平行光源の生成
	light_ = LWP::Object::CreateInstance<LWP::Object::DirectionLight>();
	light_->intensity = 7.5f;
	light_->isActive = true;

	// キャラクターマネージャーを生成
	characterManager_ = std::make_unique<CharacterManager>();
	characterManager_->Initialize();

	// カメラが追従するようにセット
	mainCamera->transform.Initialize();
	mainCamera->transform.Parent(characterManager_->GetPlayerTransform());
}

void GamePlay::Update() {
	// キャラの更新
	characterManager_->Update();

	// キルカウントが一定以上になったら -> 次のシーンへ
	if (characterManager_->killedEnemyCount_ >= 20) {
		nextScene_ = new Result();
	}
}