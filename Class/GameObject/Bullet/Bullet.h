#pragma once
#include "Model.h"
#include <WorldTransform.h>
#include "../Collision/Collider.h"

class Bullet : public Collider {
public: // メンバ関数
	
	// 初期化
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	// 更新
	void Update();
	// 描画
	void Draw(const ViewProjection& viewProjection);

	// デスフラグをチェック
	bool isDead() { return isDead_; }

	// 衝突時に呼ばれる関数
	void OnCollision() override { isDead_ = true; }

private: // メンバ定数
	// 弾の生存時間
	const int32_t kLifeTime = 60 * 5;

private: // メンバ変数
	// 弾のモデル
	Model* model_;
	
	// 速度
	Vector3 velocity_;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_;
};