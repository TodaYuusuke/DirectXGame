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
	/// 当たり判定用のSphereクラス
	/// </summary>
	class Sphere
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 中心座標
		LWP::Math::Vector3 position;
		// 半径
		float radius = 1.0f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Sphere();
		Sphere(const LWP::Math::Vector3& pos);
		Sphere(const float& rad);
		Sphere(const LWP::Math::Vector3& pos, const float& rad);
		// コピーコンストラクタ
		Sphere(const Sphere& other);

		// 更新時に形状を追従するための処理
		void Update() override;
		// 自分を囲む最小のAABBを返す関数
		void GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) override;

		// 座標を指定して生成
		void Create(const LWP::Math::Vector3& pos);
		void Create(const LWP::Math::Vector3& pos, const float& rad);
		// 形状から包み込む最小のAABBを生成する関数
		void Create(LWP::Resource::RigidModel* model);

		// 形状を返す
		Shape GetShape() override { return Shape::Sphere; }
		// ImGuiの派生クラス
		void DebugGUI() override;

#if DEMO
	private:
		// デバッグ用モデル
		//LWP::Primitive::Sphere sphereModel;
#endif

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

		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 中心座標
			LWP::Math::Vector3 position;
			// 半径
			float radius;

			// コンストラクタ
			Data(Sphere& sphere);
		};
	};
};