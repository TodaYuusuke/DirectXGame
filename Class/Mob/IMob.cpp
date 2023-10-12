#include "IMob.h"

using namespace LWP::Math;

/// 更新
void IMob::Update() {
	// 死んでいるなら戻る
	if (!isActive_) { return; }

	// 更新処理
	Moving();
	Flying();

	// 限界は絶対に超えないようにする
	if (velocity_.x > kLimitSpeed) { velocity_.x = kLimitSpeed; }
	if (velocity_.x < -kLimitSpeed) { velocity_.x = -kLimitSpeed; }
	if (velocity_.z > kLimitSpeed) { velocity_.z = kLimitSpeed; }
	if (velocity_.z < -kLimitSpeed) { velocity_.z = -kLimitSpeed; }
	// 速度の限界値を設定しておく
	if (velocity_.x > GetMaxSpeed() || velocity_.x < -GetMaxSpeed()) { velocity_.x *= 0.9f; }
	if (velocity_.z > GetMaxSpeed() || velocity_.z < -GetMaxSpeed()) { velocity_.z *= 0.9f; }

	// 速度を付与
	model_->transform.translation += velocity_;

	// もし地面を貫通したなら修正して反発する
	if (model_->transform.translation.y < 0.0f) {
		model_->transform.translation.y = 0.0f;
		velocity_.y *= -GetBounceFactor();
		// 上方向への速度が一定以下になったならバウンドをやめる
		if (velocity_.y <= 0.002f) {
			velocity_.y = 0.0f;
			isFlying_ = false;
		}
	}


	// 影とモデルの座標は常に同期する
	shadow_->transform.translation.x = model_->transform.translation.x;
	shadow_->transform.translation.z = model_->transform.translation.z;
}

void IMob::Moving() {
	// 浮遊中ならば戻る
	if (isFlying_) { return; }

}

void IMob::Flying() {
	// 浮遊中でなければ戻る
	if (!isFlying_) { return; }

	// 重力を加算
	velocity_.y -= kGravityAccele;
}
