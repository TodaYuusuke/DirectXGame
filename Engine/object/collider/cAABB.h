#include "ICollider.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のAABBクラス
	/// </summary>
	class AABB 
		: public ICollider {
	public:
		// 最小
		LWP::Math::Vector3 min;
		// 最大
		LWP::Math::Vector3 max;


	public: // ** 各形状に対する当たり判定 ** //
		bool CheckCollision(AABB* c) override;
		bool CheckCollision(OBB* c) override;
		bool CheckCollision(Sphere* c) override;

		// observer用のオペレーターオーバーロード
		bool operator==(const AABB& other) const {
			return { *followPtr_ == *other.followPtr_ &&
				min == other.min &&
				max == other.max
			};
		}
	};
};