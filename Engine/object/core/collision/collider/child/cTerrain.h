#pragma once
#include "../ICollider.h"
#include "../../OctreeSpaceDivision.h"

// 前方宣言
namespace LWP {
	namespace Resource { class StaticModel; }
}

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のTerrainクラス
	/// </summary>
	class Terrain final
		: public ICollider {
	public: // ** 内包クラス ** //
		struct Polygon {
			Math::Vector3 pos[3];	// 3点
			Math::Vector3 normal;	// 向いている法線
		};
		// モートン序列番号（8分木空間分割）ごとにリスト化されたポリゴンのマップ
		std::map<int, std::list<Polygon>> polygonMap_;

	public: // ** メンバ関数 ** //
		// コンストラクタ
		Terrain();
		// コピーコンストラクタ
		Terrain(const Terrain& other);

		// 固有の更新処理
		void Update() override;
		// 自分を囲む最小のAABBを返す関数
		void GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) override;

		// Staticモデルと適応するwtfからコライダーを生成
		void Create(const Resource::StaticModel& model);

		// octreeのポインタをセット
		void SetOctree(Object::OctreeSpaceDivision* octree) { octree_ = octree; }
		// 形状を返す
		Shape GetShape() override { return Shape::Terrain; }
		// ImGuiの派生クラス
		void DebugGUI() override;

	private:
		// octreeのポインタ
		Object::OctreeSpaceDivision* octree_;
		// このモデルを囲うAABB
		LWP::Math::Vector3 min_;
		LWP::Math::Vector3 max_;


	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(Point& c, Math::Vector3* fixVec) override;
		bool CheckCollision(AABB& c, Math::Vector3* fixVec) override;
		//bool CheckCollision(OBB& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Sphere& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Capsule& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Mesh& c, Math::Vector3* fixVec)  override;
		bool CheckCollision(Terrain& c, Math::Vector3* fixVec)  override;

		// ヒット時の処理をまとめた関数
		void Hit() override;
	};


};