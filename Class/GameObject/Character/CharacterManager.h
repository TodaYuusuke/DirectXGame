#pragma once
#include "./Enemy/Enemy.h"
#include "./Player/Player.h"
#include "../Collision/CollisionManager.h"
#include "../MyClass/Utility/TimedCall/TimedCall.h"

class CharacterManager {
public: // メンバ関数
	CharacterManager() = default;
	~CharacterManager();

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw(const ViewProjection& viewProjection);
	void DrawUI();

	// プレイヤーのWorldTransformを取得
	const WorldTransform& GetPlayerWorldTransform() { return player_->GetWorldTransform(); }

	// 雑魚敵の撃破数
	int32_t killedEnemyCount_;

private: // メンバ変数
	// プレイヤー
	std::unique_ptr<Player> player_;
	// 雑魚敵たち
	std::list<Enemy*> enemies_;

	// 雑魚敵のモデル
	std::unique_ptr<Model> enemyCoreModel_;
	std::unique_ptr<Model> enemyBlock1Model_;
	std::unique_ptr<Model> enemyBlock2Model_;
	// 雑魚敵の被弾テクスチャハンドル
	uint32_t enemyTextureHandle_ = 0u;

	// 当たり判定マネージャー
	std::unique_ptr<CollisionManager> collisionManager_;


private: // メンバ関数
	// 敵の挙動
	enum EnemyMoveType {
		Parallel1, // 平行移動1
		Parallel2, // 平行移動2
		Parallel3, // 平行移動3
		Parallel4, // 平行移動4
		Parallel5, // 平行移動5
		Parallel6, // 平行移動6
		Parallel7, // 平行移動7
		Parallel8, // 平行移動8
		EnemyMoveTypeSize
	};
	// 敵の制御点の配列
	std::vector<Vector3> enemyControlPoints_[EnemyMoveTypeSize];
	// 敵の遅延生成
	std::vector<TimedCall<void()>*> timedCalls_;
	// 敵を生成する間隔
	int32_t kEnemySpawnCycle_ = 120;
	int32_t enemySpawnFrame_ = 0;

	// 雑魚の挙動を定義
	void SetEnemyMovement();
	// 敵を召喚
	void SummonEnemy(EnemyMoveType moveType);
	// 複数の敵を召喚
	void SummonEnemies(EnemyMoveType moveType);
};