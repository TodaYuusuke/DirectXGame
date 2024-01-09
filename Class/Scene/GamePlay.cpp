#include "GamePlay.h"
#include <AxisIndicator.h>
#include "Result.h"

void GamePlay::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 3Dモデルデータ
	skydomeModel_.reset(Model::CreateFromOBJ("skydome", true));
	groundModel_.reset(Model::CreateFromOBJ("ground", true));

	// ビュープロジェクション
	viewProjection_.Initialize();

	// スカイドームの生成
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());
	// グラウンドの生成
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(groundModel_.get());

	// キャラクターマネージャーを生成
	characterManager_ = std::make_unique<CharacterManager>();
	characterManager_->Initialize();

	// 一人称カメラの生成
	fpsCamera_ = std::make_unique<FPSCamera>();
	fpsCamera_->Initialize();
	fpsCamera_->SetTarget(&characterManager_->GetPlayerWorldTransform());
}

void GamePlay::Update() {
	// カメラ更新
	fpsCamera_->Update();
	ViewProjection v = fpsCamera_->GetViewProjection();
	viewProjection_.matView = v.matView;
	viewProjection_.matProjection = v.matProjection;
	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	// キャラの更新
	characterManager_->Update();

	// キルカウントが一定以上になったら -> 次のシーンへ
	if (characterManager_->killedEnemyCount_ >= 20) {
		nextScene_ = new Result();
	}
}

void GamePlay::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// スカイドームの描画
	skydome_->Draw(viewProjection_);
	// グラウンドの描画
	ground_->Draw(viewProjection_);

	// キャラの描画
	characterManager_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	characterManager_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}