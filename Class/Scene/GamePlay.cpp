#include "GamePlay.h"
//#include "Result.h"

void GamePlay::Initialize() {
	// スカイドームの生成
	skydome_ = LWP::Resource::LoadModel("skydome/skydome.obj");
	skydome_->transform.scale = { 500, 500, 500 };
	// グラウンドの生成
	ground_ = LWP::Resource::LoadModel("ground/ground.obj");
	ground_->transform.scale = { 50, 50, 50 };

	// キャラクターマネージャーを生成
	characterManager_ = std::make_unique<CharacterManager>();
	characterManager_->Initialize();

	// カメラが追従するようにセット
	mainCamera->transform.Initialize();
	mainCamera->transform.Parent(characterManager_->GetPlayerTransform());

	//// 一人称カメラの生成
	//fpsCamera_ = std::make_unique<FPSCamera>();
	//fpsCamera_->Initialize();
	//fpsCamera_->SetTarget(&characterManager_->GetPlayerWorldTransform());
}

void GamePlay::Update() {
	//// カメラ更新
	//fpsCamera_->Update();
	//ViewProjection v = fpsCamera_->GetViewProjection();
	//viewProjection_.matView = v.matView;
	//viewProjection_.matProjection = v.matProjection;
	//// ビュープロジェクション行列の転送
	//viewProjection_.TransferMatrix();

	//// キャラの更新
	characterManager_->Update();

	//// キルカウントが一定以上になったら -> 次のシーンへ
	//if (characterManager_->killedEnemyCount_ >= 20) {
	//	nextScene_ = new Result();
	//}
}