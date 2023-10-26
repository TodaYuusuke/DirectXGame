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
	retriButtonTex_ = LWP::Resource::LoadTexture("UI/title.png");
	titleButtonTex_ = LWP::Resource::LoadTexture("UI/retry.png");
	scoreUITex_ = LWP::Resource::LoadTexture("UI/score.png");
	timeUITex_ = LWP::Resource::LoadTexture("UI/time.png");

	// イージング用ワールドトランスフォーム
	retriWorldTransform_[0].translation = { 780.0f,-200.0f,0.0f };
	retriWorldTransform_[1].translation = { 780.0f,480.0f,0.0f };
	titleWorldTransform_[0].translation = { 200.0f,-200.0f,0.0f };
	titleWorldTransform_[1].translation = { 200.0f,480.0f,0.0f };
	fullSize.scale = { 1.2f, 1.2f, 1.0f };
	miniSize.scale = { 0.4f, 0.4f, 1.0f };


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
	numberTex_[10] = LWP::Resource::LoadTexture("UI/colon.png");

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

	// 文字系
	scoreSpriteUI_ = LWP::Primitive::CreateInstance<Surface>();
	scoreSpriteUI_->texture = scoreUITex_;
	scoreSpriteUI_->vertices[0].position = { 0.0f,0.0f,0.0f };
	scoreSpriteUI_->vertices[1].position = { 384.0f,0.0f,0.0f };
	scoreSpriteUI_->vertices[2].position = { 384.0f,192.0f,0.0f };
	scoreSpriteUI_->vertices[3].position = { 0.0f,192.0f,0.0f };
	scoreSpriteUI_->isUI = true;
	scoreSpriteUI_->isActive = true;
	scoreSpriteUI_->transform.translation = { 80.0f,60.0f,0.0f };

	timeSpriteUI_ = LWP::Primitive::CreateInstance<Surface>();
	timeSpriteUI_->texture = timeUITex_;
	timeSpriteUI_->vertices[0].position = { 0.0f,0.0f,0.0f };
	timeSpriteUI_->vertices[1].position = { 384,0.0f,0.0f };
	timeSpriteUI_->vertices[2].position = { 384.0f,192.0f,0.0f };
	timeSpriteUI_->vertices[3].position = { 0.0f,192.0f,0.0f };
	timeSpriteUI_->isUI = true;
	timeSpriteUI_->isActive = true;
	timeSpriteUI_->transform.translation = { 80.0f,260.0f,0.0f };


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
		scoreUI_[i]->transform.translation = { i * 80.0f + 410,60.0f,0.0f };
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
		timeUI_[i]->transform.translation = { (i * 80.0f) + 830,260.0f,0.0f };
	}
	timeUI_[0]->transform.translation.x -= 40.0f;

	colonUI_ = LWP::Primitive::CreateInstance<Surface>();
	colonUI_->texture = numberTex_[10];
	colonUI_->vertices[0].position = { 0.0f,0.0f,0.0f };
	colonUI_->vertices[1].position = { 108.0f,0.0f,0.0f };
	colonUI_->vertices[2].position = { 108.0f,108.0f,0.0f };
	colonUI_->vertices[3].position = { 0.0f,108.0f,0.0f };
	colonUI_->isUI = true;
	colonUI_->isActive = true;
	colonUI_->transform.translation = { 870.0f,290.0f,0.0f };

	#pragma endregion

	// UI
	#pragma region result
	
	titleButton_ = LWP::Primitive::CreateInstance<Surface>();
	titleButton_->vertices[0].position = { 0.0f,0.0f };
	titleButton_->vertices[1].position = { 320.0f,0.0f };
	titleButton_->vertices[2].position = { 320.0f,160.0f };
	titleButton_->vertices[3].position = { 0.0f,160.0f };
	titleButton_->isUI = true;
	titleButton_->isActive = true;
	titleButton_->texture = titleButtonTex_;
	titleButton_->transform.translation = titleWorldTransform_[0].translation;
	
	retriButton_ = LWP::Primitive::CreateInstance<Surface>();
	retriButton_->vertices[0].position = { 0.0f,0.0f };
	retriButton_->vertices[1].position = { 320.0f,0.0f };
	retriButton_->vertices[2].position = { 320.0f,160.0f };
	retriButton_->vertices[3].position = { 0.0f,160.0f };
	retriButton_->isUI = true;
	retriButton_->isActive = true;
	retriButton_->texture = retriButtonTex_;
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
			if (Input::Controller::GetTrigger(DIXBOX_A)) {
				t[0] = 1.0f;
			}

			t[0] += (1.0f / 196.0f);
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
			if (Input::Controller::GetTrigger(DIXBOX_A)) {
				t[1] = 1.0f;
			}

			t[1] += (1.0f / 128);
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
			if (Input::Controller::GetTrigger(DIXBOX_A)) {
				t[2] = 1.0f;
			}

			t[2] += (1.0f / 128.0f);
			if (t[2] > 1.0f) { t[2] = 1.0f; }
			
			retriButton_->transform = Easing::EaseOutBounce(retriWorldTransform_[0], retriWorldTransform_[1], t[2]);
			titleButton_->transform = Easing::EaseOutBounce(titleWorldTransform_[0], titleWorldTransform_[1], t[2]);
		}
		else {

			if (Keyboard::GetTrigger(DIK_A)) {
				isGoTitle = false;
			}
			if (Keyboard::GetTrigger(DIK_D)) {
				isGoTitle = true;
			}

			if (Input::Controller::GetLStick().x <= -0.9f) {
				isGoTitle = false;

			}
			else if (Input::Controller::GetLStick().x >= 0.9f) {
				isGoTitle = true;

			}

			// ボタンのイージング
			if (isSelectUp) {
				selectT += 0.01f;
				if (selectT > 1.0f) {
					isSelectUp = false;
					selectT = 1.0f;
				}
			}
			else {
				selectT -= 0.01f;
				if (selectT < 0.0f) {
					isSelectUp = true;
					selectT = 0.0f;
				}
			}

			// 選択中のボタンを拡縮
			if (!isGoTitle) {
				titleButton_->transform.scale = Easing::ScaleEaseOutQuint(miniSize, fullSize, selectT).scale;
				retriButton_->transform.scale = { 1.0f,1.0f,1.0f };
			}
			else {
				retriButton_->transform.scale = Easing::ScaleEaseOutQuint(miniSize, fullSize, selectT).scale;
				titleButton_->transform.scale = { 1.0f,1.0f,1.0f };
			}

			// ENTERキーを押すとシーン切り替え
			if (Keyboard::GetTrigger(DIK_RETURN)) {
				isTransitionSceneEnd_ = true;
			}

			if (Input::Controller::GetTrigger(DIXBOX_A)) {
				isTransitionSceneEnd_ = true;
			}

		}

		//// カメラ操作
		//ImGui::Begin("Camera");
		//mainCamera->DebugGUI();
		//ImGui::End();

	}

}

// 描画
void GameResult::Draw() {

}
