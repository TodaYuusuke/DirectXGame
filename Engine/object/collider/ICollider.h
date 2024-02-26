#include "object/WorldTransform.h"

namespace LWP::Object::Collider {
	// 前方宣言
	class AABB;
	class OBB;
	class Sphere;

	/// <summary>
	/// 当たり判定用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ関数 ** //
		// デストラクタ
		~ICollider() = default;

		// セットする関数
		void SetFollowTarget(LWP::Object::WorldTransform* ptr) { followPtr_ = ptr; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void AdjustPosition(const LWP::Math::Vector3& fixVector) { followPtr_->translation += fixVector; }


	public: // ** 各形状に対する当たり判定 ** //
		virtual bool CheckCollision(AABB* c) { return false; }
		virtual bool CheckCollision(OBB* c) { return false; }
		virtual bool CheckCollision(Sphere* c) { return false; }

	protected: // ** 継承用変数 ** //
		// 追従するワールドトランスフォーム
		LWP::Object::WorldTransform* followPtr_ = nullptr;

		// 動くかのフラグ
		bool isMove = true;
		// 前フレーム当たっていたかのフラグ
		bool preHit = false;
	};
};