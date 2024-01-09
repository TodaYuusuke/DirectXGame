#include "CollisionManager.h"

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != --colliders_.end(); ++itrA) {
		// イテレータBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			CheckCollisionPair(*itrA, *itrB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) ||
	    !(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask())) {
		return;
	}

	// 球と球の交差判定
	if (Distance(colliderA->GetWorldPosition(), colliderB->GetWorldPosition()) <=
	    powf((colliderA->GetRadius() + colliderB->GetRadius()), 2)) {
		// 衝突時コールバックを呼び出す
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}