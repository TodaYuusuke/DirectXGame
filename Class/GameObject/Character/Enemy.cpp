#include "Enemy.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Resource;

void Enemy::Initialize(Vector3 position) {
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);
	// 衝突半径を設定
	SetRadius(5.0f);

	// enemyのモデル読み込み
	coreModel_ = LoadModel("enemy_core/enemy_core.obj");
	coreModel_->transform.translation = position;
	coreModel_->material.enableLighting = true;
	coreTex_ = coreModel_->texture.t;
	block1Model_ = LoadModel("enemy_block1/enemy_block1.obj");
	block1Model_->material.enableLighting = true;
	block1Tex_ = coreModel_->texture.t;
	block2Model_ = LoadModel("enemy_block2/enemy_block2.obj");
	block2Model_->material.enableLighting = true;
	block2Tex_ = coreModel_->texture.t;

	hitTex_ = LoadTextureLongPath("resources/system/texture/white.png");

	// 親子関連付け
	block1Model_->transform.Parent(&coreModel_->transform);
	block2Model_->transform.Parent(&coreModel_->transform);

	// コライダー用のトランスフォームをセット
	worldTransformPtr_ = &coreModel_->transform;
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
	coreModel_->transform.translation = Vector3::GetCatmullRomPosition(controlPoints_, t);

	Vector3 forword = playerWorldTransform_->GetWorldPosition() - coreModel_->transform.translation;
	// Y軸周りの角度
	coreModel_->transform.rotation.y = std::atan2f(forword.x, forword.z);
	// X軸周りの角度
	coreModel_->transform.rotation.x = std::atan2f(-forword.y, Vector3{forword.x, 0, forword.z}.Length());
	
	// テクスチャを元に張り替える
	if (kHitFrame_ <= 0) {
		coreModel_->texture.t = coreTex_;
		coreModel_->material.enableLighting = true;
		block1Model_->texture.t = block1Tex_;
		block1Model_->material.enableLighting = true;
		block2Model_->texture.t = block2Tex_;
		block2Model_->material.enableLighting = true;
	}

	// アニメーション
	Rotation();
	Elasticity();
	Hit();
}

void Enemy::OnCollision() {
	health--;
	if (health <= 0) {
		*killCount_ += 1;
		isDead_ = true;
		coreModel_->isActive = false;
		block1Model_->isActive = false;
		block2Model_->isActive = false;
	}
	// 被弾リアクションを実行
	kHitFrame_ = 5;

	coreModel_->texture.t = hitTex_;
	coreModel_->material.enableLighting = false;
	block1Model_->texture.t = hitTex_;
	block1Model_->material.enableLighting = false;
	block2Model_->texture.t = hitTex_;
	block2Model_->material.enableLighting = false;

	EffectManager::GetInstance()->Start(coreModel_->transform.GetWorldPosition());
}


void Enemy::SetModelNeutral() {
	coreModel_->transform.translation = { 0.0f, 0.0f, 0.0f };
	coreModel_->transform.rotation = {0.0f, 0.0f, 0.0f};
	coreModel_->transform.scale = { 0.5f, 0.5f, 0.5f };
	block1Model_->transform.translation = {0.0f, 0.0f, 0.0f};
	block1Model_->transform.rotation = {0.0f, 0.0f, 0.0f};
	block1Model_->transform.scale = {1.0f, 1.0f, 1.0f};
	block2Model_->transform.translation = {0.0f, 0.0f, 0.0f};
	block2Model_->transform.rotation = {0.0f, 0.0f, 0.0f};
	block2Model_->transform.scale = {1.0f, 1.0f, 1.0f};
}
void Enemy::ApplyGlobalVariables() {

}

void Enemy::Rotation() {
	block1Model_->transform.rotation += kRotationSpeed_;
	block2Model_->transform.rotation += kRotationSpeed_;
}
void Enemy::Elasticity() {
	block1Model_->transform.scale = Vector3::Slerp(
	    {kElasticityMINSize_, kElasticityMINSize_, kElasticityMINSize_},
	    {kElasticityMAXSize_, kElasticityMAXSize_, kElasticityMAXSize_}, kElasticityT_);
	block2Model_->transform.scale = Vector3::Slerp(
	    {kElasticityMINSize_, kElasticityMINSize_, kElasticityMINSize_},
	    {kElasticityMAXSize_, kElasticityMAXSize_, kElasticityMAXSize_}, 1.0f - kElasticityT_);

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
