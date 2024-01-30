#include "PlayerBullet.h"

using namespace LWP::Math;

void PlayerBullet::Initialize(const Vector3& position, const Vector3& velocity) { 
	// モデル
	model_ = LWP::Resource::LoadModel("cube/cube.obj");
	// テクスチャ読み込み
	model_->texture = LWP::Resource::LoadTexture("bullet.png");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	// 引数で受け取った初期座標をセット
	model_->transform.translation = position;

	// 引数で受け取った速度を代入
	velocity_ = velocity;

	model_->transform.scale = { 0.5f,0.5f,0.5f };
}

void PlayerBullet::Update() {
	// 座標を移動させる
	model_->transform.translation += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isAlive_ = false;
		model_->isActive = false;
	}
}


void PlayerBullet::OnCollision() { 
	isAlive_ = false;
	model_->isActive = false;
}

Vector3 PlayerBullet::GetWorldPosition() { return model_->transform.GetWorldPosition(); }