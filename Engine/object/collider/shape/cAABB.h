#pragma once
#include "ICollisionShape.h"

#if DEMO
#include "primitive/3d/Cube.h"
#endif

// 前方宣言
namespace LWP::Resource {
	class RigidModel;
}

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のAABBクラス
	/// </summary>
	class AABB final
		: public ICollisionShape {
	public: // ** パブリックなメンバ変数 ** //
		// 最小
		LWP::Math::Vector3 min = { -0.5f,-0.5f,-0.5f };
		// 最大
		LWP::Math::Vector3 max = { 0.5f,0.5f,0.5f };


	public: // ** メンバ関数 ** //
		// コンストラクタ
		AABB();
		AABB(const LWP::Math::Vector3& min, const LWP::Math::Vector3& max);
		// コピーコンストラクタ
		AABB(const AABB& other);

		// 固有の更新処理
		void Update() override;

		// 場所を指定して生成する関数
		void Create(const LWP::Math::Vector3& position);
		void Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size);
		// 形状から包み込む最小のAABBを生成する関数
		void Create(const LWP::Resource::RigidModel& model);

		// 形状を返す
		Shape GetShape() override { return Shape::AABB; }
		// ImGuiの派生クラス
		void DebugGUI() override;

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Cube cube;
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

		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 最小
			LWP::Math::Vector3 min;
			// 最大
			LWP::Math::Vector3 max;
			// 中心の座標
			LWP::Math::Vector3 center;

			// コンストラクタ
			Data(AABB& aabb);
		};
	};


};