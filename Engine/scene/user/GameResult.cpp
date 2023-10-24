#include "GameResult.h"
#include "GameScene.h"
#include "TItle.h"
#include "../Class/Ease/Easing.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Input;

// 初期化
void GameResult::Initialize() {

	// トランジションフラグをtrueに
	isTransition_ = true;
	isTransitionSceneEnd_ = false;

	// テクスチャ読み込み
	transitionTex_ = LWP::Resource::LoadTexture("Transition.png");
	buttonTex_ = LWP::Resource::LoadTexture("white.png");

	// 平面
	for (size_t i = 0; i < 2; i++)
	{
		transitionSurfase_[i] = LWP::Primitive::CreateInstance<Surface>();
		transitionSurfase_[i]->vertices[0].position = { 0.0f,0.0f };
		transitionSurfase_[i]->vertices[1].position = { 1280.0f,0.0f };
		transitionSurfase_[i]->vertices[2].position = { 1280.0f,720.0f };
		transitionSurfase_[i]->vertices[3].position = { 0.0f,720.0f };
		transitionSurfase_[i]->isUI = true;
		transitionSurfase_[i]->isActive = true;
		transitionSurfase_[i]->texture = transitionTex_;
	}

	transitionSurfase_[0]->transform.translation = { -640.0f,0.0f,0.0f };
	transitionSurfase_[1]->transform.translation = { 640.0f,0.0f,0.0f };

	// イージング用ワールドトランスフォーム
	scoreWorldTransform_[0].translation = { 100.0f,-200.0f,0.0f };
	scoreWorldTransform_[1].translation = { 100.0f,400.0f,0.0f };
	timeWorldTransform_[0].translation = { 500.0f,-200.0f,0.0f };
	timeWorldTransform_[1].translation = { 500.0f,200.0f,0.0f };
	resultWorldTransform_[0].translation = { 800.0f,-200.0f,0.0f };
	resultWorldTransform_[1].translation = { 800.0f,600.0f,0.0f };

	// スコア表示UI
	#pragma region resultScore

	resultScore_ = LWP::Primitive::CreateInstance<Surface>();
	resultScore_->vertices[0].position = { 0.0f,0.0f };
	resultScore_->vertices[1].position = { 320.0f,0.0f };
	resultScore_->vertices[2].position = { 320.0f,160.0f };
	resultScore_->vertices[3].position = { 0.0f,160.0f };
	resultScore_->isUI = true;
	resultScore_->isActive = true;
	resultScore_->texture = transitionTex_;
	resultScore_->transform.translation = scoreWorldTransform_[0].translation;

	#pragma endregion	

	// タイム表示UI
	#pragma region resultTime

	resultTime_ = LWP::Primitive::CreateInstance<Surface>();
	resultTime_->vertices[0].position = { 0.0f,0.0f };
	resultTime_->vertices[1].position = { 320.0f,0.0f };
	resultTime_->vertices[2].position = { 320.0f,160.0f };
	resultTime_->vertices[3].position = { 0.0f,160.0f };
	resultTime_->isUI = true;
	resultTime_->isActive = true;
	resultTime_->texture = transitionTex_;
	resultTime_->transform.translation = timeWorldTransform_[0].translation;

	#pragma endregion

	// タイム表示UI
	#pragma region result

	result_ = LWP::Primitive::CreateInstance<Surface>();
	result_->vertices[0].position = { 0.0f,0.0f };
	result_->vertices[1].position = { 320.0f,0.0f };
	result_->vertices[2].position = { 320.0f,160.0f };
	result_->vertices[3].position = { 0.0f,160.0f };
	result_->isUI = true;
	result_->isActive = true;
	result_->texture = transitionTex_;
	result_->transform.translation = resultWorldTransform_[0].translation;

	#pragma endregion
}

// 更新
void GameResult::Update() {

	// 別のシーンからの遷移実行時
	if (isTransition_) {

		// 矩形が画面外に出るまで移動させる
		if (transitionSurfase_[1]->transform.translation.x < 1280.0f) {
			transitionSurfase_[0]->transform.translation.x -= 10.0f;
			transitionSurfase_[1]->transform.translation.x += 10.0f;
		}
		else {
			isTransition_ = false;
		}

	}
	// 別のシーンへの遷移実行時
	else if(isTransitionSceneEnd_){

		// 矩形が画面外に出るまで移動させる
		if (transitionSurfase_[1]->transform.translation.x >= 640.0f) {
			transitionSurfase_[0]->transform.translation.x += 10.0f;
			transitionSurfase_[1]->transform.translation.x -= 10.0f;
		}
		else {

			// フラグに応じてシーンに遷移
			if (isGoTitle) {
				nextScene_ = new Title();
			}
			else {
				nextScene_ = new GameScene();
			}
		}

		
	}
	else {
		
		
		if (t[0] < 1.0f) {
		
			t[0] += (1.0f / 128.0f);
			if (t[0] > 1.0f) { t[0] = 1.0f; }

			resultScore_->transform = Easing::EaseOutBounce(
				scoreWorldTransform_[0],
				scoreWorldTransform_[1],
				t[0]
			);

		}else if (t[1] < 1.0f) {

			t[1] += (1.0f / 128.0f);
			if (t[1] > 1.0f) { t[1] = 1.0f; }

			resultTime_->transform = Easing::EaseOutBounce(
				timeWorldTransform_[0],
				timeWorldTransform_[1],
				t[1]
			);

		}else if (t[2] < 1.0f) {

			t[2] += (1.0f / 128.0f);
			if (t[2] > 1.0f) { t[2] = 1.0f; }

			result_->transform = Easing::EaseOutBounce(
				resultWorldTransform_[0],
				resultWorldTransform_[1],
				t[2]
			);

		}
		else {
			// ENTERキーを押すとシーン切り替え
			if (Keyboard::GetTrigger(DIK_RETURN)) {
				isTransitionSceneEnd_ = true;
			}
		}

		// Rキーを押すとシーン切り替え
		if (Keyboard::GetTrigger(DIK_R)) {
			nextScene_ = new GameResult;
		}

		// カメラ操作
		ImGui::Begin("Camera");
		mainCamera->DebugGUI();
		ImGui::End();

	}

}

// 描画
void GameResult::Draw() {

}
