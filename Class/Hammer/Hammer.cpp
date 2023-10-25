#include "Hammer.h"

using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Utility;

void Hammer::Initialize(MapManager* map) {
	// タンバリンの初期化
	tambourine_.Initialize();

	// レティクル初期化
	reticle_ = CreateInstance<Surface>();
	reticle_->transform.translation.y = 0.01f;
	reticle_->transform.rotation.x = 1.57f;
	reticle_->transform.scale = { 0.3f, 0.3f, 0.3f };
	reticle_->texture = LoadTexture("reticle.png");

	// サウンドの初期化
	impactSE_ = LWP::Resource::LoadAudio("impact02.wav");
	impactSE_->SetLoopCount(0);
	bellSE_ = LWP::Resource::LoadAudio("impact03.wav");
	bellSE_->SetLoopCount(0);

	// モデルの初期化
	model_ = LoadModel("hammer/hammer.obj");
	model_->transform.translation.y = 1.0f;
	model_->transform.scale = { 0.15f, 0.15f, 0.15f };
	// 半透明に
	model_->commonColor = new Color(0xFFFFFF64);


	// マップのポインタ受け取り
	mapPtr_ = map;
}

void Hammer::Update() {
	UpdateTimer();

	ImGui::Begin("Hammer");
	model_->DebugGUI("Hammer");
	reticle_->DebugGUI("reticle");
	ImGui::End();

	// タンバリンの更新
	tambourine_.Update();

	// レティクル移動
	keyBoard();
	Controller();

	// ハンマーとレティクルの座標は常に同期する
	model_->transform.translation.x = reticle_->transform.translation.x;
	model_->transform.translation.z = reticle_->transform.translation.z;
}

void Hammer::UpdateTimer() {
	attackCoolTimer >= 0 ? attackCoolTimer-- : 0;
	attackAni.Progress(&model_->transform);
}

void Hammer::keyBoard() {
	LWP::Math::Vector2 move{ 0.0f,0.0f };
	// WASDでレティクル移動
	if (Keyboard::GetPress(DIK_W)) {
		move.y += 1.0f;
	}
	if (Keyboard::GetPress(DIK_A)) {
		move.x -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_S)) {
		move.y -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_D)) {
		move.x += 1.0f;
	}

	// レティクルの移動処理
	move *= kReticleSpeed;
	reticle_->transform.translation.x += move.x;
	reticle_->transform.translation.z += move.y;

	// SPACEで攻撃
	if (Keyboard::GetTrigger(DIK_SPACE) && attackCoolTimer < 0) {
		Attack();
		// タンバリンをシェイクさせる
		tambourine_.ShakeBell();
	}
}

void Hammer::Controller() {
	LWP::Math::Vector2 move{ 2.0f,2.0f };
	// スティックでレティクル移動

	if (Controller::GetLStick().x || Controller::GetLStick().y) {
		LStickVelocity = Controller::GetLStick();
		// レティクルの移動処理
		move *= kReticleSpeed;
		reticle_->transform.translation.x += LStickVelocity.x * move.x;
		reticle_->transform.translation.z -= LStickVelocity.y * move.y;

	}

	//移動処理
	centerX = 0.0f;
	centerY = 0.0f;
	radius = 1.0f;

	deltaX = reticle_->transform.translation.x - centerX;
	deltaY = reticle_->transform.translation.z - centerY;
	if (deltaX * deltaX + deltaY * deltaY > radius * radius) {
		// 円の境界を超えた場合は移動しないようにする
		float angle = atan2(deltaY, deltaX);
		reticle_->transform.translation.x = centerX + cos(angle) * radius;
		reticle_->transform.translation.z = centerY + sin(angle) * radius;
	}

		// Aボタンで攻撃
	if (Controller::GetTrigger(DIXBOX_A) && attackCoolTimer < 0) {
		Attack();
		// タンバリンをシェイクさせる
		tambourine_.ShakeBell();
	}
	
}

void Hammer::Attack() {
	// アニメーション開始
	attackAni.timer = attackAni.GetFullTIme();

	// 攻撃クールタイム
	attackCoolTimer = 30;
	// 攻撃を発生させる
	mapPtr_->AddShockWave(reticle_->transform.translation);

	// SEを流す
	impactSE_->Play();
	impactSE_->SetVolume(0.6f);
	bellSE_->Play();
	bellSE_->SetVolume(0.6f);

}


void Hammer::AttackAnimation::Progress(LWP::Object::WorldTransform* transform) {
	timer--;
	if (timer < 0) { return; }

	// 振り下ろしアニメーション
	if (timer >= swingUp.time) {
		*transform += swingDown.transform / static_cast<float>(swingDown.time);
	}
	else {
		*transform += swingUp.transform / static_cast<float>(swingUp.time);
	}
}