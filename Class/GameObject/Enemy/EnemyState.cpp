	#include "EnemyState.h"

// 初期化（EnemyApproach）
void EnemyApproach::Initialize(Enemy* enemy) {

	enemy->Fire();
	//enemy->fireTimer_ = enemy->kFireInterval;
}
// 更新処理（EnemyApproach）
void EnemyApproach::Update(Enemy* enemy) {
	// 経過時間更新
	elapsedFrame++;
	// 移動
	enemy->Move(kApproachSpeed);
	// コールバックを更新
	enemy->timedCall_->Update();

	// 一定以上プレイヤーに近づいたら or 一定時間生存したら離脱
	if (enemy->GetEnemyToPlayerDistance() < 100.0f || elapsedFrame > 60 * 7) {
		enemy->ChangePhase(new EnemyLeave());
	}
}


// 初期化（EnemyApproach）
void EnemyLeave::Initialize(Enemy* enemy) {
	// 現段階では処理なし
	enemy;
}
// 更新処理（EnemyLeave）
void EnemyLeave::Update(Enemy* enemy) {
	// 経過時間更新
	elapsedFrame++;
	// 移動
	enemy->Move(kLeaveSpeed);

	// 一定時間たったら死亡
	if (elapsedFrame > 180) {
		enemy->OnCollision();
	}
};