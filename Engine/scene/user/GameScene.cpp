#include "GameScene.h"
#include "GameResult.h"
#include "../Class/Ease/Easing.h"

using namespace LWP;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void GameScene::Initialize() {
	// カメラを上から見下ろす形に
	mainCamera->transform.translation = { 0.0f,4.5f,0.0f };
	mainCamera->transform.rotation = { 1.57f,0.0f,0.0f };
	// 前から見たカメラ
	//mainCamera->transform.translation = { 0.0f,3.5f,-3.0f };
	//mainCamera->transform.rotation = { 0.85f,0.0f,0.0f };
	
	skydome_.Initialze();
	mapManager_.Initialize();
	hammer_.Initialize(&mapManager_);
	mobManager_.Initialize();

	// サウンド
	gameBGM_ = LWP::Resource::LoadAudio("battle01.wav");
	gameBGM_->SetLoopCount(255);
	gameBGM_->Play();
	bgmVol_ = 0.0f;
	gameBGM_->SetVolume(bgmVol_);

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

	// スコアUI
	score_ = 0;
	scoreEaseT = 0.0f;
	calcScore_[0], calcScore_[1] = 0;
	viewScore_ = 0;

	for (int32_t i = 0; i < 8; i++) {
		scoreUI_[i] = LWP::Primitive::CreateInstance<Surface>();
		scoreUI_[i]->texture = numberTex_[0];
		scoreUI_[i]->vertices[0].position = { 0.0f,0.0f,0.0f };
		scoreUI_[i]->vertices[1].position = { 48.0f,0.0f,0.0f };
		scoreUI_[i]->vertices[2].position = { 48.0f,48.0f,0.0f };
		scoreUI_[i]->vertices[3].position = { 0.0f,48.0f,0.0f };
		scoreUI_[i]->isUI = true;
		scoreUI_[i]->isActive = true;
		scoreUI_[i]->transform.translation = { i * 24.0f,20.0f,0.0f };
	}

	time_ = 3600;
	nextSceneWaitTime_ = 300;

	// タイムUI
	for (int32_t i = 0; i < 3; i++) {
		timeUI_[i] = LWP::Primitive::CreateInstance<Surface>();
		timeUI_[i]->texture = numberTex_[0];
		timeUI_[i]->vertices[0].position = { 0.0f,0.0f,0.0f };
		timeUI_[i]->vertices[1].position = { 64.0f,0.0f,0.0f };
		timeUI_[i]->vertices[2].position = { 64.0f,64.0f,0.0f };
		timeUI_[i]->vertices[3].position = { 0.0f,64.0f,0.0f };
		timeUI_[i]->isUI = true;
		timeUI_[i]->isActive = true;
		timeUI_[i]->transform.translation = { (i * 36.0f) + 1120.0f,20.0f,0.0f };
	}
	timeUI_[0]->transform.translation.x -= 20.0f;

	colonUI_ = LWP::Primitive::CreateInstance<Surface>();
	colonUI_->texture = numberTex_[10];
	colonUI_->vertices[0].position = { 0.0f,0.0f,0.0f };
	colonUI_->vertices[1].position = { 48.0f,0.0f,0.0f };
	colonUI_->vertices[2].position = { 48.0f,48.0f,0.0f };
	colonUI_->vertices[3].position = { 0.0f,48.0f,0.0f };
	colonUI_->isUI = true;
	colonUI_->isActive = true;
	colonUI_->transform.translation = { 1135.0f,30.0f,0.0f };

}
// 更新
void GameScene::Update() {

	mapManager_.Update();


	ImGui::Begin("GameScene");
	ImGui::Text("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
	
	// 時間がなくなったら or ターゲットが死亡したら終了
	if (time_ <= 0 || mobManager_.GetTargetDead()) {

		if (bgmVol_ >= 0.0f) {
			bgmVol_ -= 0.005f;
			if (bgmVol_ <= 0.0f) { gameBGM_->Stop(); }
			if (bgmVol_ < 0.0f) { bgmVol_ = 0.0f; }
			gameBGM_->SetVolume(bgmVol_);
		}

		// ゲーム終了してから演出用の時間を設ける
		if (nextSceneWaitTime_ >= 0) {
			nextSceneWaitTime_--;
		}
		else {

			// 護衛対象が生存していればクリア判定をtrueに
			if (!mobManager_.GetTargetDead()) { isClear_ = true; }
			nextScene_ = new GameResult(score_, time_);
			gameBGM_->Stop();

		}

	}
	else {
		if (bgmVol_ < 0.5f) {
			bgmVol_ += 0.005f;
			gameBGM_->SetVolume(bgmVol_);
		}

		if (Tutorial == 0) {
			time_--;
		}

		// 敵の出現
		if ((time_ % 350) == 0) {
			// 基本3体
			mobManager_.AddEnemy(3);
			if (time_ <= 3000) {
				// 後半で5体に
				mobManager_.AddEnemy(2);
			}
		}

		hammer_.Update();
		mobManager_.Update(&mapManager_);
		score_ = mobManager_.score_;

		// 終点の値と現在スコア値が違う場合
		if (calcScore_[1] != score_) {
			// 始点の値に現在表示されている値を代入
			calcScore_[0] = viewScore_;
			// 終点の値を代入する
			calcScore_[1] = score_;
			// イージングの位置をリセット
			scoreEaseT = 0.0f;
		}

	}
	
	// スコアの表示をイージングする
	if (scoreEaseT < 1.0f) {
		scoreEaseT += 0.025f;
		if (scoreEaseT > 1.0f) { scoreEaseT = 1.0f; }
	}
	viewScore_ = Easing::EaseinQuad(calcScore_[0], calcScore_[1], scoreEaseT);
	

	Reset();

	// タイマーとスコアのUIへの変換を行う
	timeUI_[0]->texture = numberTex_[(time_ / 60) / 60];
	timeUI_[1]->texture = numberTex_[((time_ / 60) % 60)/10];
	timeUI_[2]->texture = numberTex_[((time_ / 60) % 60) % 10];
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

	// Cキーを押すと敵召喚
	if (Input::Keyboard::GetTrigger(DIK_C)) {
		mobManager_.AddEnemy(1);
	}

	// Rキーを押すとシーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextScene_ = new GameScene();
		gameBGM_->Stop();
	}

	// ENTERキーを押すとシーン切り替え(ゲームリザルト)
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {

		nextScene_ = new GameResult(score_,time_);
		gameBGM_->Stop();
	}

}
// 描画
void GameScene::Draw() {}

LWP::Math::Vector3 GameScene::Lerp(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, float t) {
	// tが0の場合はstart、tが1の場合はendを返す
	if (t <= 0.0f) {
		return start;
	}
	if (t >= 1.0f) {
		return end;
	}
	// それ以外の場合は線形補間を行う
	return start + t * (end - start);
};

void GameScene::Reset() {
	LWP::Math::Vector3 offset = { 1.5f, 0.0f, 2.5f };

	LWP::Math::Matrix4x4 cameraRotateMatrix = LWP::Math::Matrix4x4::CreateRotateXYZMatrix(mainCamera->transform.rotation);

	offset = LWP::Math::Matrix4x4::TransformCoord(offset, cameraRotateMatrix);

	LWP::Math::Vector3 destinationangle = { 0.0f,0.0f,0.0f };

	// カメラの移動
	if (Input::Controller::GetRStick().x || Input::Controller::GetRStick().y) {
		RStickVelocity = Input::Controller::GetRStick();

		mainCamera->transform.translation.x = -RStickVelocity.x * offset.x; 
		mainCamera->transform.translation.z = -RStickVelocity.y * offset.y; 

		destinationangle.z += RStickVelocity.x * 0.4f; 
		destinationangle.x += RStickVelocity.y * 0.4f + 1.57f;
	}
	else { // カメラの位置をリセット
		mainCamera->transform.translation.x = 0.0f;
		mainCamera->transform.translation.z = 0.0f;

		destinationangle.x = 1.57f; // x軸回転の初期値
		destinationangle.z = 0.0f; // y軸回転の初期値
	}

	// 最短角度補間
	mainCamera->transform.rotation = Lerp(mainCamera->transform.rotation, destinationangle, 0.1f); // 0.4fは補間係数
}