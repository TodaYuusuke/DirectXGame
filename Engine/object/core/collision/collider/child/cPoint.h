#pragma once
#include "../ICollider.h"

#if DEMO
//#include "primitive/3d/Sphere.h"
#endif

// 前方宣言
namespace LWP::Resource {
	class RigidModel;
}

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のPointクラス
	/// </summary>
	class Point final
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 座標
		Math::Vector3 position;

	public: // ** メンバ関数 ** //
		// コンストラクタ
		Point();
		Point(const LWP::Math::Vector3& pos);
		// コピーコンストラクタ
		Point(const Point& other);

		// 固有の更新処理
		void Update() override;
		// 自分を囲む最小のAABBを返す関数
		void GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) override;

		// 形状を返す
		Shape GetShape() override { return Shape::Point; }
		// ImGuiの派生クラス
		void DebugGUI() override;

#if DEMO
	private:
		// デバッグ用モデル
		//LWP::Primitive::Sphere sphere;
#endif

	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(Point& c, Math::Vector3* fixVec) override;
		bool CheckCollision(AABB& c, Math::Vector3* fixVec) override;
		//bool CheckCollision(OBB& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Sphere& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Capsule& c, Math::Vector3* fixVec)  override;

		// ヒット時の処理をまとめた関数
		void Hit() override;

		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 座標
			LWP::Math::Vector3 position;

			// コンストラクタ
			Data(Point& point);
		};
	};


};