#pragma once
#include "Collider.h"
#include <list>

class CollisionManager {
public: // メンバ関数

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();
	
	/// <summary>
	/// リストをクリア
	/// </summary>
	void ClearList() { colliders_.clear(); }
	/// <summary>
	/// リストにコライダーを登録
	/// </summary>
	void PushCollider(Collider* collider) { colliders_.push_back(collider); }

private: // 非公開の関数

	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	// 二つのベクトル間の距離を求める
	float Distance(LWP::Math::Vector3 v1, LWP::Math::Vector3 v2) {
		return powf((v2.x - v1.x), 2) + powf((v2.y - v1.y), 2) + powf((v2.z - v1.z), 2);
	}

private: // メンバ変数
	std::list<Collider*> colliders_;
};