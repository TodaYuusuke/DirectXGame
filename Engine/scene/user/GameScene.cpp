#include "GameScene.h"
#include "GameResult.h"

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
	
	tambourine_.Initialize();
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

	// スコアUI
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

	time_ = 10800;
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
			nextScene_ = new GameResult();

		}

	}
	else {
		if (bgmVol_ < 0.7f) {
			bgmVol_ += 0.005f;
			gameBGM_->SetVolume(bgmVol_);
		}

		time_--;
		hammer_.Update();
		mobManager_.Update(&mapManager_);

	}

	Reset();

	// タイマーとスコアのUIへの変換を行う
	timeUI_[0]->texture = numberTex_[(time_ / 60) / 60];
	timeUI_[1]->texture = numberTex_[((time_ / 60) % 60)/10];
	timeUI_[2]->texture = numberTex_[((time_ / 60) % 60) % 10];

	// Cキーを押すと敵召喚
	if (Input::Keyboard::GetTrigger(DIK_C)) {
		mobManager_.AddEnemy(1);
	}

	// Rキーを押すとシーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextScene_ = new GameScene();
	}	

	// ENTERキーを押すとシーン切り替え(ゲームリザルト)
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new GameResult();
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
	LWP::Math::Vector3 offset = { 0.1f, 0.0f, 0.1f };

	LWP::Math::Matrix4x4 cameraRotateMatrix = LWP::Math::Matrix4x4::CreateRotateXYZMatrix(mainCamera->transform.rotation);

	offset = LWP::Math::Matrix4x4::TransformCoord(offset, cameraRotateMatrix);

	LWP::Math::Vector3 destinationangle = { 0.0f,0.0f,0.0f };

	// カメラの移動
	if (Input::Controller::GetLStick().x || Input::Controller::GetLStick().y) {
		LStickVelocity = Input::Controller::GetLStick();

		mainCamera->transform.translation.x = -LStickVelocity.x * offset.x;
		mainCamera->transform.translation.z = -LStickVelocity.y * offset.y;

		destinationangle.z += LStickVelocity.x * 0.1f; // 0.2fは傾ける角度
		destinationangle.x += LStickVelocity.y * 0.1f + 1.57f;;

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