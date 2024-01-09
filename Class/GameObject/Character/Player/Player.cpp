#include "Player.h"
#include <ImGuiManager.h>
#include <cassert>
#include <Input.h>
#include "../../../GlobalVariables/GlobalVariables.h"

Player::~Player() {
	for (Bullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize(const std::vector<Model*>& models, Vector3 position) {
	BaseCharacter::Initialize(models, position);

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	// 弾丸のモデルを登録
	bulletModel_.reset(Model::CreateFromOBJ("bullet", true));

	// ハンドガン初期化
	handgun_ = std::make_unique<HandGun>();
	handgun_->Initialize(worldTransform_);

	// モデルのWorldTransformの初期化
	SetModelNeutral();
}

void Player::Update() {
	// コントローラーでの操作
	MoveJoyStick();
	// リコイルを徐々に元に戻す
	recoil_ = Slerp(recoil_, {0.0f, 0.0f, 0.0f}, 0.1f);
	// ハンドガン更新
	handgun_->Update();
	// 弾丸更新
	UpdateBullets();

	// 調整項目適応
	ApplyGlobalVariables();
	BaseCharacter::Update();
}

void Player::Draw(const ViewProjection& viewProjection) {
	for (int i = 0; i < models_.size(); i++) {
		models_[i].model_->Draw(models_[i].worldTransform_, viewProjection);
	}
	
	// ハンドガン描画
	handgun_->Draw(viewProjection);

	// 弾描画
	for (Bullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
void Player::DrawUI() {
	handgun_->DrawUI();
}

void Player::MoveJoyStick() {
	// ゲームパッドの状態を得る変数（XINPUT）
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// RがTriggerならば or RTがPressならば >>> 弾を発射
		if ((!preRS && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) || (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)) {
			if (handgun_->Shot(&recoil_)) {	// ハンドガンを射撃
				// 弾の速度
				Vector3 velocity =
				    handgun_->GetReticleWorldPosition() - worldTransform_.GetWorldPosition();
				velocity = Normalize(velocity) * kBulletSpeed;

				// 弾を発射
				Bullet* newBullet = new Bullet();
				newBullet->Initialize(
				    bulletModel_.get(), worldTransform_.GetWorldPosition(), velocity);
				bullets_.push_back(newBullet);
			}
		}
		// Lトリガーを押していたら >>> ADS
		if (!preLS && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
			handgun_->SwitchADS();
		}
		// Xトリガーを押していたら >>> リロード
		if (!preX && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			handgun_->Reload();
		}
		// 入力を保存
		preRS = joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		preLS = joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		preX = joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
		

		// 視点移動
		Vector3 turn = {
		    -(float)joyState.Gamepad.sThumbRY / SHRT_MAX,
		    (float)joyState.Gamepad.sThumbRX / SHRT_MAX, 0.0f};
		if (handgun_->GetisADS())	// ADS中なら速度を下げる
			rotation_ += turn * (kCameraTurnSpeed * 0.7f);
		else 
			rotation_ += turn * kCameraTurnSpeed;
		// 角度制限
		rotation_.x = std::clamp(rotation_.x, -1.3f, 0.7f);
		// 最後にworldTransformにセット
		worldTransform_.rotation_ = rotation_ + recoil_;


		// キャラクター移動
		Vector3 move = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		move *= kCharacterSpeed;
		Vector3 rotation = worldTransform_.rotation_;
		rotation.x = 0.0f;
		move = move * Matrix4x4::MakeRotateMatrix(rotation);
		// 移動
		worldTransform_.translation_ += move;
		// 範囲制限
		worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -40.0f, 40.0f);
		worldTransform_.translation_.z = std::clamp(worldTransform_.translation_.z, -40.0f, 40.0f);
	}
}
void Player::UpdateBullets() {
	// 弾更新
	for (Bullet* bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を削除
	bullets_.remove_if([](Bullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
}

void Player::SetModelNeutral() {
}
void Player::ApplyGlobalVariables() {
}
