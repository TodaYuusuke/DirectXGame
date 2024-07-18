#pragma once
#include "ICollisionShape.h"
#include <list>

#if DEMO
#include "primitive/2d/Triangle.h"
#endif

// 前方宣言
namespace LWP::Resource {
	class RigidModel;
	class StaticModel;
}

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のMeshクラス
	/// </summary>
	class Mesh final
		: public ICollisionShape {
	public: // ** パブリックなメンバ変数 ** //
		// 三角形
		struct TriangleData {
			Math::Vector3 pos[3];
			Math::Vector3 normal;
		};
		std::vector<TriangleData> data;

	public: // ** メンバ関数 ** //
		// コンストラクタ
		Mesh();
		// コピーコンストラクタ
		Mesh(const Mesh& other);

		// 固有の更新処理
		void Update() override;

		// StaticModelから当たり判定を生成
		void Create(const LWP::Resource::StaticModel& model);

		// 形状を返す
		Shape GetShape() override { return Shape::Mesh; }
		// ImGuiの派生クラス
		void DebugGUI() override;

	private:
		// 当たり判定用のモデル
		Resource::StaticModel* model_;
#if DEMO
		// デバッグ用モデル
		std::list<Primitive::Triangle> triangles_;
#endif

	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(Point& c) override;
		bool CheckCollision(AABB& c) override;
		//bool CheckCollision(OBB& c)  override;
		bool CheckCollision(Sphere& c)  override;
		bool CheckCollision(Capsule& c)  override;
		bool CheckCollision(Mesh& c)  override;

		// ヒット時の処理をまとめた関数
		void Hit() override;
	};


};