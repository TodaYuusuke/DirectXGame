#include "EnemyBullet.h"
#include "../Player/Player.h"

using namespace LWP::Math;

void EnemyBullet::Initialize(const Vector3& position, const Vector3& velocity) {
	// モデル読み込み
	model_ = LWP::Resource::LoadModel("cube/cube.obj");
	// テクスチャ読み込み
	model_->texture = LWP::Resource::LoadTexture("EnemyBullet.png");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);

	// 引数で受け取った初期座標をセット
	model_->transform.translation = position;
	model_->transform.scale = {0.2f,0.2f,0.5f};

	// 引数で受け取った速度を代入
	velocity_ = velocity;

}

void EnemyBullet::Update() {

	// プレイヤーの方向へ誘導
	velocity_ = Vector3::Slerp(
		velocity_.Normalize(),
		(player_->GetWorldPosition() - model_->transform.translation).Normalize(),
		0.01f
	);
	// Y軸周りの角度
	model_->transform.rotation.y = std::atan2f(velocity_.x, velocity_.z);
	// X軸周りの角度
	model_->transform.rotation.x = std::atan2f(-velocity_.y, Vector3{ velocity_.x, 0.0f, velocity_.z}.Length());

	// 座標を移動させる
	model_->transform.translation += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isAlive_ = false;
		model_->isActive = false;
	}
}

void EnemyBullet::OnCollision() { 
	isAlive_ = false;
	model_->isActive = false;
}

Vector3 EnemyBullet::GetWorldPosition() { return model_->transform.GetWorldPosition(); }