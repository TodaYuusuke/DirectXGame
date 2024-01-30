#include "Enemy.h"
#include <cassert>
#include "../Player/Player.h"
#include "../../scene/GameScene.h"

using namespace LWP;
using namespace LWP::Math;

// デストラクタ
Enemy::~Enemy() {
	delete phase_;
	delete timedCall_;
}

/// <summary>
/// 初期化
/// </summary>
void Enemy::Initialize(Vector3 position) {
	// モデル
	model_ = LWP::Resource::LoadModel("cube/cube.obj");
	model_->texture = LWP::Resource::LoadTexture("enemy.png");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);

	// ワールドトランスフォ－ムの初期化
	model_->transform.translation = position;
	
	// 最初の行動フェーズ
	ChangePhase(new EnemyApproach());
}

/// <summary>
/// 更新
/// </summary>
void Enemy::Update() {
	if (!isAlive_) {
		return;
	}

	// 行動フェーズごとの更新処理
	phase_->Update(this);
}

void Enemy::OnCollision() {
	// 何もしない
	isAlive_ = false;
	model_->isActive = false;
}
Vector3 Enemy::GetWorldPosition() { return model_->transform.GetWorldPosition(); }

/*ーーーーーーーーーーー*/
/*　　　その他関数　　　*/
/*ーーーーーーーーーーー*/


// 行動フェーズを変更する
void Enemy::ChangePhase(BaseEnemyState* newState) {
	delete phase_;
	phase_ = newState;
	// 初期化
	phase_->Initialize(this);
}

// 弾を発射
void Enemy::Fire() {
	// 弾の速度
	Vector3 velocity{};

	velocity = player_->GetWorldPosition() - GetWorldPosition();
	velocity = velocity.Normalize() * 0.5f;
	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(GetWorldPosition(), velocity);
	newBullet->SetPlayer(player_);

	// 弾を登録する
	gameScene_->AddEnemyBullet(newBullet);

	std::function<void()> f = std::bind(&Enemy::Fire, this);
	timedCall_ = new TimedCall<void()>(f, 30);
}

float Enemy::GetEnemyToPlayerDistance() {
	return Vector3::Distance(GetWorldPosition(), player_->GetWorldPosition());
}
