#pragma once
#include "ICollisionShape.h"
#include <list>
#include <vector>

#include "math/vector/Vector3.h"

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
			Math::Vector3 center;
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

	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(Point& c, Math::Vector3* fixVec) override;
		bool CheckCollision(AABB& c, Math::Vector3* fixVec) override;
		//bool CheckCollision(OBB& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Sphere& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Capsule& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Mesh& c, Math::Vector3* fixVec)  override;

		// ヒット時の処理をまとめた関数
		void Hit() override;
	};


};