#include "ICollider.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のOBBクラス
	/// </summary>
	class OBB
		: public ICollider {
	public:
		// 座標軸
		LWP::Math::Vector3 orientations[3];
		// サイズ
		LWP::Math::Vector3 size;


	public: // ** 各形状に対する当たり判定 ** //
		bool CheckCollision(AABB* c) override;
		bool CheckCollision(OBB* c) override;
		bool CheckCollision(Sphere* c) override;

		// Observer用のオペレーターオーバーロード
		bool operator==(const OBB& other) const {
			return { *followPtr_ == *other.followPtr_ &&
				orientations[0] == other.orientations[0] &&
				orientations[1] == other.orientations[1] &&
				orientations[2] == other.orientations[2] &&
				size == other.size
			};
		}

	private: // ** プライベートな関数 ** //
		// ImGuiの派生クラス
		void DerivedDebugGUI() override;
	};
};