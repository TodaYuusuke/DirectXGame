#pragma once
#include "../ICollider.h"
#include "primitive/3d/Sphere.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のSphereクラス
	/// </summary>
	class Sphere
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 半径
		float radius = 0.5f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Sphere();
		Sphere(const float& rad);

		// 更新時に形状を追従するための処理
		void Update() override;
		// 自分を囲む最小のAABBを返す関数
		void GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) override;

		// 形状から包み込む最小のSphereを生成する関数
		void Create(LWP::Resource::RigidModel* model);

		// 形状を返す
		Shape GetShape() override { return Shape::Sphere; }
		// ImGuiの派生クラス
		void DebugGUI() override;

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Sphere dPri;
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
			// 中心座標
			LWP::Math::Vector3 position;
			// 半径
			float radius;

			// コンストラクタ
			Data(Sphere& sphere);
		};
	};
};