#include "Enemy.h"
#include <ImGuiManager.h>
#include <cassert>
#include <Input.h>

void Enemy::Initialize(const std::vector<Model*>& models, Vector3 position) {
	BaseCharacter::Initialize(models, position);

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);
	// 衝突半径を設定
	SetRadius(5.0f);

	// 親子関連付け
	models_[Core].worldTransform_.parent_ = &worldTransform_;
	models_[Block1].worldTransform_.parent_ = &worldTransform_;
	models_[Block2].worldTransform_.parent_ = &worldTransform_;
}

void Enemy::Update() {
	if (isDead_) { return; }

	// レールに沿って進む
	t += 0.001f;
	if (t > 1.0f) {
		isDead_ = true;
		return;
	}
	// 座標を計算
	worldTransform_.translation_ = GetCatmullRomPosition(controlPoints_, t);

	Vector3 forword = playerWorldTransform_->GetWorldPosition() - worldTransform_.translation_;
	// Y軸周りの角度
	models_[Core].worldTransform_.rotation_.y = std::atan2f(forword.x, forword.z);
	// X軸周りの角度
	models_[Core].worldTransform_.rotation_.x = std::atan2f(-forword.y, Length({forword.x, 0, forword.z}));
	
	// アニメーション
	Rotation();
	Elasticity();
	Hit();

	BaseCharacter::Update();
}
void Enemy::Draw(const ViewProjection& viewProjection) {
	if (isDead_) {
		return;
	}

	for (int i = 0; i < models_.size(); i++) {
		if (kHitFrame_ > 0) {
			models_[i].model_->Draw(models_[i].worldTransform_, viewProjection, hitTextureHandle_);
		} else {
			models_[i].model_->Draw(models_[i].worldTransform_, viewProjection);
		}
	}
}

void Enemy::OnCollision() {
	health--;
	if (health <= 0) {
		*killCount_ += 1;
		isDead_ = true;
	}
	// 被弾リアクションを実行
	kHitFrame_ = 5;
}


void Enemy::SetModelNeutral() {
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	models_[Core].worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	models_[Core].worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	models_[Core].worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	models_[Block1].worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	models_[Block1].worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	models_[Block1].worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	models_[Block2].worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	models_[Block2].worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	models_[Block2].worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
}
void Enemy::ApplyGlobalVariables() {

}

void Enemy::Rotation() {
	models_[Block1].worldTransform_.rotation_ += kRotationSpeed_;
	models_[Block2].worldTransform_.rotation_ += kRotationSpeed_;
}
void Enemy::Elasticity() {
	models_[Block1].worldTransform_.scale_ = Slerp(
	    {kElasticityMINSize_, kElasticityMINSize_, kElasticityMINSize_},
	    {kElasticityMAXSize_, kElasticityMAXSize_, kElasticityMAXSize_}, kElasticityT_);
	models_[Block2].worldTransform_.scale_ = Slerp(
	    {kElasticityMINSize_, kElasticityMINSize_, kElasticityMINSize_},
	    {kElasticityMAXSize_, kElasticityMAXSize_, kElasticityMAXSize_}, 1.0f - kElasticityT_);
	;

	if (kElasticityUpper) {
		kElasticityT_ += 1.0f / (float)kElasticityCycleFrame_;
		if (kElasticityT_ > 1.0f) {
			kElasticityT_ = 1.0f;
			kElasticityUpper = false;
		}
	}
	else {
		kElasticityT_ -= 1.0f / (float)kElasticityCycleFrame_;
		if (kElasticityT_ < 0.0f) {
			kElasticityT_ = 0.0f;
			kElasticityUpper = true;
		}
	}
}

void Enemy::Hit() {
	if (kHitFrame_ > 0) {
		kHitFrame_--;
	}
}
