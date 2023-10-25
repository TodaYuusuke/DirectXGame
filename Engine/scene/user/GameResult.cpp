#include "GameResult.h"
#include "GameScene.h"
#include "TItle.h"
#include "../Class/Ease/Easing.h"

using namespace LWP;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Input;

GameResult::GameResult(int32_t score, int32_t time) {
	score_ = score;
	time_ = 10800 - time;
}

// 初期化
void GameResult::Initialize() {

	// トランジションフラグをtrueに
	isTransition_ = true;
	isTransitionSceneEnd_ = false;

	// テクスチャ読み込み
	transitionTex_ = LWP::Resource::LoadTexture("Transition.png");
	buttonTex_ = LWP::Resource::LoadTexture("white.png");
	backgroundTex_ = LWP::Resource::LoadTexture("white.png");

	// イージング用ワールドトランスフォーム
	retriWorldTransform_[0].translation = { 200.0f,-200.0f,0.0f };
	retriWorldTransform_[1].translation = { 200.0f,480.0f,0.0f };
	titleWorldTransform_[0].translation = { 780.0f,-200.0f,0.0f };
	titleWorldTransform_[1].translation = { 780.0f,480.0f,0.0f };

	// 数値テクスチャ
	numberTex_[0] = LWP::Resource::LoadTexture("UI/0.png");
	numberTex_[1] = LWP::Resource::LoadTexture("UI/1.png");
	numberTex_[2] = LWP::Resource::LoadTexture("UI/2.png");
	numberTex_[3] = LWP::Resource::LoadTexture("UI/3.png");
	numberTex_[4] = LWP::Resource::LoadTexture("UI/4.png");
	numberTex_[5] = LWP::Resource::LoadTexture("UI/5.png");
	numberTex_[6] = LWP::Resource::LoadTexture("UI/6.png");
	numberTex_[7] = LWP::Resource::LoadTexture("UI/7.png");
	numberTex_[8] = LWP::Resource::LoadTexture("UI/8.png");
	numberTex_[9] = LWP::Resource::LoadTexture("UI/9.png");

	// 背景
	background_ = LWP::Primitive::CreateInstance<Surface>();
	background_->texture = backgroundTex_;
	background_->vertices[0].position = { 0.0f,0.0f,0.0f };
	background_->vertices[1].position = { 1280.0f,0.0f,0.0f };
	background_->vertices[2].position = { 1280.0f,720.0f,0.0f };
	background_->vertices[3].position = { 0.0f,720.0f,0.0f };
	background_->isUI = true;
	background_->isActive = true;
	background_->commonColor = new Color(0x999999FF);

	// スコア表示UI
	#pragma region resultScore
	viewScore_ = 0;

	for (int32_t i = 0; i < 8; i++) {
		scoreUI_[i] = LWP::Primitive::CreateInstance<Surface>();
		scoreUI_[i]->texture = numberTex_[0];
		scoreUI_[i]->vertices[0].position = { 0.0f,0.0f,0.0f };
		scoreUI_[i]->vertices[1].position = { 192.0f,0.0f,0.0f };
		scoreUI_[i]->vertices[2].position = { 192.0f,192.0f,0.0f };
		scoreUI_[i]->vertices[3].position = { 0.0f,192.0f,0.0f };
		scoreUI_[i]->isUI = true;
		scoreUI_[i]->isActive = true;
		scoreUI_[i]->transform.translation = { i * 80.0f + 400,60.0f,0.0f };
	}

	#pragma endregion	

	// タイム表示UI
	#pragma region resultTime
	viewTime = 0;

	for (int32_t i = 0; i < 3; i++) {
		timeUI_[i] = LWP::Primitive::CreateInstance<Surface>();
		timeUI_[i]->texture = numberTex_[0];
		timeUI_[i]->vertices[0].position = { 0.0f,0.0f,0.0f };
		timeUI_[i]->vertices[1].position = { 160.0f,0.0f,0.0f };
		timeUI_[i]->vertices[2].position = { 160.0f,160.0f,0.0f };
		timeUI_[i]->vertices[3].position = { 0.0f,160.0f,0.0f };
		timeUI_[i]->isUI = true;
		timeUI_[i]->isActive = true;
		timeUI_[i]->transform.translation = { (i * 80.0f) + 820,260.0f,0.0f };
	}
	timeUI_[0]->transform.translation.x -= 20.0f;

	#pragma endregion

	// UI
	#pragma region result
	titleButton_;
	retriButton_;

	titleButton_ = LWP::Primitive::CreateInstance<Surface>();
	titleButton_->vertices[0].position = { 0.0f,0.0f };
	titleButton_->vertices[1].position = { 320.0f,0.0f };
	titleButton_->vertices[2].position = { 320.0f,160.0f };
	titleButton_->vertices[3].position = { 0.0f,160.0f };
	titleButton_->isUI = true;
	titleButton_->isActive = true;
	titleButton_->texture = transitionTex_;
	titleButton_->transform.translation = titleWorldTransform_[0].translation;
	
	retriButton_ = LWP::Primitive::CreateInstance<Surface>();
	retriButton_->vertices[0].position = { 0.0f,0.0f };
	retriButton_->vertices[1].position = { 320.0f,0.0f };
	retriButton_->vertices[2].position = { 320.0f,160.0f };
	retriButton_->vertices[3].position = { 0.0f,160.0f };
	retriButton_->isUI = true;
	retriButton_->isActive = true;
	retriButton_->texture = transitionTex_;
	retriButton_->transform.translation = retriWorldTransform_[0].translation;

	#pragma endregion

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
		
		// スコアイージング
		if (t[0] < 1.0f) {
		
			// ENTERキーを押すとシーン切り替え
			if (Keyboard::GetTrigger(DIK_RETURN)) {
				t[0] = 1.0f;
			}

			t[0] += (1.0f / 512.0f);
			if (t[0] > 1.0f) { t[0] = 1.0f; }

			// スコア表示をイージング
			viewScore_ = Easing::EaseinQuad(0, score_, t[0]);

			// スコア
			scoreUI_[0]->texture = numberTex_[(viewScore_ / 10000000)];
			viewScore_ = viewScore_ % 10000000;
			scoreUI_[1]->texture = numberTex_[(viewScore_ / 1000000)];
			viewScore_ = viewScore_ % 1000000;
			scoreUI_[2]->texture = numberTex_[(viewScore_ / 100000)];
			viewScore_ = viewScore_ % 100000;
			scoreUI_[3]->texture = numberTex_[(viewScore_ / 10000)];
			viewScore_ = viewScore_ % 10000;
			scoreUI_[4]->texture = numberTex_[(viewScore_ / 1000)];
			viewScore_ = viewScore_ % 1000;
			scoreUI_[5]->texture = numberTex_[(viewScore_ / 100)];
			viewScore_ = viewScore_ % 100;
			scoreUI_[6]->texture = numberTex_[(viewScore_ / 10)];
			viewScore_ = viewScore_ % 10;
			scoreUI_[7]->texture = numberTex_[(viewScore_)];
			

		}else if (t[1] < 1.0f) {

			// ENTERキーを押すとシーン切り替え
			if (Keyboard::GetTrigger(DIK_RETURN)) {
				t[1] = 1.0f;
			}

			t[1] += (1.0f / 256);
			if (t[1] > 1.0f) { t[1] = 1.0f; }

			// スコア表示をイージング
			viewTime = Easing::EaseinQuad(0, time_, t[1]);

			// タイマーとスコアのUIへの変換を行う
			timeUI_[0]->texture = numberTex_[(viewTime / 60) / 60];
			timeUI_[1]->texture = numberTex_[((viewTime / 60) % 60) / 10];
			timeUI_[2]->texture = numberTex_[((viewTime / 60) % 60) % 10];


		}
		else if (t[2] < 1.0f) {

			// ENTERキーを押すとシーン切り替え
			if (Keyboard::GetTrigger(DIK_RETURN)) {
				t[2] = 1.0f;
			}

			t[2] += (1.0f / 128.0f);
			if (t[2] > 1.0f) { t[2] = 1.0f; }
			
			retriButton_->transform = Easing::EaseOutBounce(retriWorldTransform_[0], retriWorldTransform_[1], t[2]);
			titleButton_->transform = Easing::EaseOutBounce(titleWorldTransform_[0], titleWorldTransform_[1], t[2]);
		}
		else {
			// ENTERキーを押すとシーン切り替え
			if (Keyboard::GetTrigger(DIK_RETURN)) {
				isTransitionSceneEnd_ = true;
			}
		}

		// Rキーを押すとシーン切り替え
		if (Keyboard::GetTrigger(DIK_R)) {
			nextScene_ = new GameResult(score_, time_);
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
