#include "Bullet.h"

void Bullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	// 衝突半径を設定
	SetRadius(0.05f);

	model_ = model;

	// ワールドトランスフォ－ムの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// 引数で受け取った速度を代入
	velocity_ = velocity;
	// Y軸周りの角度
	worldTransform_.rotation_.y = std::atan2f(velocity_.x, velocity_.z);
	// X軸周りの角度
	worldTransform_.rotation_.x = std::atan2f(-velocity_.y, Length({velocity_.x, 0, velocity_.z}));

	worldTransform_.scale_.x = 0.5f;
	worldTransform_.scale_.y = 0.5f;
	worldTransform_.scale_.z = 0.5f;
}

void Bullet::Update() {

	// 座標を移動させる
	worldTransform_.translation_ += velocity_;

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void Bullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}