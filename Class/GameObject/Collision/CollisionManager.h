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

private: // メンバ変数
	std::list<Collider*> colliders_;
};