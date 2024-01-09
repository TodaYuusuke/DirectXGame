#pragma once
#include "../Collision/Collider.h"

class Bullet : public Collider {
public: // メンバ関数
	
	// 初期化
	void Initialize(const LWP::Math::Vector3& position, const LWP::Math::Vector3& velocity);
	// 更新
	void Update();

	// デスフラグをチェック
	bool isDead() { return isDead_; }

	// 衝突時に呼ばれる関数
	void OnCollision() override { isDead_ = true; }

private: // メンバ定数
	// 弾の生存時間
	const int32_t kLifeTime = 60 * 5;

private: // メンバ変数
	// 弾のモデル
	LWP::Primitive::Mesh* model_;
	
	// 速度
	LWP::Math::Vector3 velocity_;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_;
};