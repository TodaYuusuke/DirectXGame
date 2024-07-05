#pragma once
#include "ICollisionShape.h"

#if DEMO
#include "primitive/3d/Sphere.h"
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
		: public ICollisionShape {
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

		// 形状を返す
		Shape GetShape() override { return Shape::Point; }
		// ImGuiの派生クラス
		void DebugGUI() override;

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Sphere sphere;
#endif

	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(Point& c) override;
		bool CheckCollision(AABB& c) override;
		//bool CheckCollision(OBB& c)  override;
		bool CheckCollision(Sphere& c)  override;
		bool CheckCollision(Capsule& c)  override;

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