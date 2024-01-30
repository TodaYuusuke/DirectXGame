#pragma once
#include "CollisionConfig.h"
#include <math/vector/Vector3.h>

class Collider {
public: // メンバ関数

	// 衝突時に呼ばれる関数
	virtual void OnCollision() = 0;
	// ワールド座標を取得
	virtual LWP::Math::Vector3 GetWorldPosition() = 0;


// *** プロパティ変数 *** //

private: // 衝突半径
	float radius_ = 1;
public: // アクセッサ
	float GetRadius() { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }

private: // 衝突属性（自分）
	uint32_t CollisionAttribute_ = 0xffffffff;
public: // アクセッサ
	uint32_t GetCollisionAttribute() { return CollisionAttribute_; }
	void SetCollisionAttribute(uint32_t attribute) { CollisionAttribute_ = attribute; }

private: // 衝突マスク（相手）
	uint32_t CollisionMask_ = 0xffffffff;
public: // アクセッサ
	uint32_t GetCollisionMask() { return CollisionMask_; }
	void SetCollisionMask(uint32_t mask) { CollisionMask_ = mask; }
};