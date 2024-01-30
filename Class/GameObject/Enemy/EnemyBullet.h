#pragma once
#include <Adapter.h>

#include "../Collision/Collider.h"

class Player;

class EnemyBullet : public Collider {
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const LWP::Math::Vector3& position, const LWP::Math::Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision() override;
	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	LWP::Math::Vector3 GetWorldPosition() override;

	// isAliveを受け取る
	bool isAlive() const { return isAlive_; }
	// プレイヤーをセット
	void SetPlayer(Player* player) { player_ = player; }

	//*　公開する定数　*//
	
	// 当たり判定の半径
	const float kCollisionRadius = 0.5f;

private: // メンバ変数
	//*　　定　数　　*//

	static const int32_t kLifeTime = 60 * 5;

	//*　　変　数　　*//

	// モデル
	LWP::Primitive::Mesh* model_ = nullptr;

	// 速度
	LWP::Math::Vector3 velocity_;
	// プレイヤー
	Player* player_;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isAlive_ = true;
};
