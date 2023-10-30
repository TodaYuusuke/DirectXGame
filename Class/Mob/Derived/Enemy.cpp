#include "Enemy.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

/// 初期化
void Enemy::Initialize(){
	// モデル
	model_ = LWP::Resource::LoadModel("enemy/enemy.obj");
	model_->transform.scale = { 0.1f ,0.1f, 0.1f };
	model_->material.enableLighting = true;
	//model_->commonColor = new Color(ColorPattern::RED);
}

void Enemy::Moving() {
	// 空中の場合は移動しない
	if (isFlying_) { return; }

	// ターゲットの方向の速度を計算
	Vector3 vector = target_->translation - model_->transform.translation;
	vector = vector.Normalize();

	vector *= kAccele;

	// Y軸の速度は捨てる
	vector.y = 0.0f;

	// 速度に足す
	velocity_ += vector;
}