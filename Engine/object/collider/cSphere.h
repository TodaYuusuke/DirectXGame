#include "ICollider.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のSphereクラス
	/// </summary>
	class Sphere
		: public ICollider {
	public:
		// 半径
		float radius;


	public: // ** 各形状に対する当たり判定 ** //
		bool CheckCollision(AABB* c) override;
		bool CheckCollision(OBB* c) override;
		bool CheckCollision(Sphere* c) override;

		// observer用のオペレーターオーバーロード
		bool operator==(const Sphere& other) const {
			return { *followPtr_ == *other.followPtr_ &&
				radius == other.radius
			};
		}
	};
};