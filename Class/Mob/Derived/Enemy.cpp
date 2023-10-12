#include "Enemy.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

/// 初期化
void Enemy::Initialize(){
	// 影
	shadow_ = CreateInstance<Surface>();
	shadow_->transform.translation.y = 0.01f;
	shadow_->transform.rotation.x = 1.57f;
	shadow_->transform.scale = { 0.1f, 0.1f, 0.1f };
	shadow_->commonColor = new Color(0x00000064);

	// モデル
	model_ = LWP::Resource::LoadModel("enemy/enemy.obj");
	model_->transform.scale = { 0.1f ,0.1f, 0.1f };
	model_->commonColor = new Color(ColorPattern::RED);
}

void Enemy::Moving() {
	// 浮遊中ならば戻る
	//if (isFlying_) { return; }

	// ターゲットの方向の速度を計算
	Vector3 vector = target_->translation - model_->transform.translation;
	vector = vector.Normalize();

	// 空中の場合は倍率を下げる
	if (isFlying_) {
		vector *= kAccele * 0.3f;
	}
	else {
		vector *= kAccele;
	}

	// Y軸の速度は捨てる
	vector.y = 0.0f;

	// 速度に足す
	velocity_ += vector;
}