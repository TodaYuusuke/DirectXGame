#pragma once
#include "../ICollider.h"
#include "primitive/3d/Capsule.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のCapsuleクラス
	/// </summary>
	class Capsule
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //

		// 始点から終点へのオフセット点
		LWP::Math::Vector3 localOffset = { 0.0f,0.0f,2.0f };
		// 半径
		float radius = 0.5f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Capsule();
		Capsule(const LWP::Math::Vector3& offset);
		Capsule(const float& rad);
		Capsule(const LWP::Math::Vector3& offset, const float& rad);

		// 固有の更新処理
		void Update() override;
		// 自分を囲む最小のAABBを返す関数
		void GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) override;

		// 形状を返す
		Shape GetShape() override { return Shape::Capsule; }
		// ImGuiの派生クラス
		void DebugGUI() override;


#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Capsule dPri;
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
			// 始点
			LWP::Math::Vector3 start;
			// 終点
			LWP::Math::Vector3 end;
			// 半径
			float radius;

			// コンストラクタ
			Data(Capsule& cap);
		};
	};
};