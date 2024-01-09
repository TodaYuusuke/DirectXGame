#include "Bullet.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

void Bullet::Initialize(const Vector3& position, const Vector3& velocity) {
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	// 衝突半径を設定
	SetRadius(0.05f);

	model_ = LoadModel("bullet/bullet.obj");
	model_->transform.translation = position;
	model_->material.enableLighting = true;

	// 引数で受け取った速度を代入
	velocity_ = velocity;
	// Y軸周りの角度
	model_->transform.rotation.y = std::atan2f(velocity_.x, velocity_.z);
	// X軸周りの角度
	model_->transform.rotation.x = std::atan2f(-velocity_.y, Vector3(velocity_.x, 0, velocity_.z).Length());

	model_->transform.scale = { 0.5f,0.5f,0.5f };

	// コライダー用のトランスフォームをセット
	worldTransformPtr_ = &model_->transform;
}

void Bullet::Update() {
	// 座標を移動させる
	model_->transform.translation += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		model_->isActive = false;
		isDead_ = true;
	}
}