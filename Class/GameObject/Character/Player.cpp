#include "Player.h"
#include "../../GlobalVariables/GlobalVariables.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Resource;

Player::~Player() {
	for (Bullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize(Vector3 position) {
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	// 座標をセット
	transform_.translation = position;

	// ハンドガン初期化
	handgun_ = std::make_unique<HandGun>();
	handgun_->Initialize(&transform_);

	// モデルのWorldTransformの初期化
	SetModelNeutral();

	// コライダー用のトランスフォームをセット
	worldTransformPtr_ = &transform_;
}

void Player::Update() {
	// コントローラーでの操作
	MoveJoyStick();
	// リコイルを徐々に元に戻す
	recoil_ = Vector3::Slerp(recoil_, {0.0f, 0.0f, 0.0f}, 0.1f);
	// ハンドガン更新
	handgun_->Update();
	// 弾丸更新
	UpdateBullets();

	// 調整項目適応
	ApplyGlobalVariables();
}

void Player::MoveJoyStick() {
	// RがTriggerならば >>> 弾を発射
	if (Pad::GetTrigger(0, XBOX_RB)) {
		if (handgun_->Shot(&recoil_)) {	// ハンドガンを射撃
			// 弾の速度
			Vector3 velocity =
				handgun_->GetReticleWorldPosition() - transform_.GetWorldPosition();
			velocity = velocity.Normalize() * kBulletSpeed;

			// 弾を発射
			Bullet* newBullet = new Bullet();
			Vector3 v = transform_.GetWorldPosition();
			newBullet->Initialize(
				v, velocity);
			bullets_.push_back(newBullet);
		}
	}
	// Lを押していたら >>> ADS
	if (Pad::GetTrigger(0, XBOX_LB)) {
		handgun_->SwitchADS();
	}
	// Xトリガーを押していたら >>> リロード
	if (Pad::GetTrigger(0, XBOX_X)) {
		handgun_->Reload();
	}


	// 視点移動
	Vector3 turn = {
		-Pad::GetRStick(0).y,
		Pad::GetRStick(0).x, 0.0f };
	if (handgun_->GetisADS())	// ADS中なら速度を下げる
		rotation_ += turn * (kCameraTurnSpeed * 0.7f);
	else
		rotation_ += turn * kCameraTurnSpeed;
	// 角度制限
	rotation_.x = std::clamp(rotation_.x, -1.3f, 0.7f);
	// 最後にworldTransformにセット
	transform_.rotation = rotation_ + recoil_;


	// キャラクター移動
	Vector3 move = {
		Pad::GetLStick(0).x, 0.0f,
		Pad::GetLStick(0).y };
	move *= kCharacterSpeed;
	Vector3 rotation = transform_.rotation;
	rotation.x = 0.0f;
	move = move * Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 移動
	transform_.translation += move;
	// 範囲制限
	transform_.translation.x = std::clamp(transform_.translation.x, -40.0f, 40.0f);
	transform_.translation.z = std::clamp(transform_.translation.z, -40.0f, 40.0f);
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
