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


	// 場外でないかを検知
	// Y軸は無視して考える
	Vector2 center = { model_->transform.translation.x,model_->transform.translation.z };
	// 点と円の距離を計算
	float distance = std::sqrt(center.x * center.x + center.y * center.y);
	
	// もし地面を貫通したなら修正して反発する（ただし場外のときはそのまま落下する）
	if (distance < 1.0f) {
		if (model_->transform.translation.y < 0.0f) {
			model_->transform.translation.y = 0.0f;
			// 現在の速度に応じてダメージを与える
			int damage = static_cast<int>(velocity_.y * 100.0f * GetDamageMultiply());
			hp_ += damage;
			// 体力がなくなったなら死亡
			if (hp_ < 0) {
				model_->isActive = false;
				shadow_->isActive = false;
				isActive_ = false;
			}

			velocity_.y *= -GetBounceFactor();
			// 上方向への速度が一定以下になったならバウンドをやめる
			if (velocity_.y <= 0.002f) {
				velocity_.y = 0.0f;
				isFlying_ = false;
			}
		}
	}
	// 場外に出た場合の処理
	else {
		// 自然に外に出た場合も落下するように
		isFlying_ = true;
		// 影は非表示
		shadow_->isActive = false;
		// ある程度落ちたら殺す
		if (model_->transform.translation.y < -5.0f) {
			model_->isActive = false;
			isActive_ = false;
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
