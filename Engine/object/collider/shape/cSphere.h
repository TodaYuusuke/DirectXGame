#pragma once
#include "ICollider.h"

#if DEMO
#include "primitive/3d/Sphere.h"
#endif

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のSphereクラス
	/// </summary>
	class Sphere
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 中心座標
		LWP::Math::Vector3 position = { 0.0f,0.0f,0.0f };
		// 半径
		float radius = 1.0f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Sphere();
		Sphere(const LWP::Math::Vector3& pos);
		Sphere(const float& rad);
		Sphere(const LWP::Math::Vector3& pos, const float& rad);

		// 更新時に形状を追従するための処理
		void Update() override;
		// ImGuiの派生クラス
		void DebugGUI() override;

		// 座標を指定して生成
		void Create(const LWP::Math::Vector3& pos);
		void Create(const LWP::Math::Vector3& pos, const float& rad);
		// 形状から包み込む最小のAABBを生成する関数
		void Create(LWP::Resource::RigidModel* model);
		// 形状を返す
		Shape GetShape() override { return Shape::Sphere; }

		// Observer用（==）
		bool operator==(const Sphere& other) {
			return {
				followModel_.GetChanged() &&
				radius == other.radius
			};
		}

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Sphere sphereModel;
#endif

	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(AABB* c) override;
		//bool CheckCollision(OBB* c)  override;
		bool CheckCollision(Sphere* c)  override;
		bool CheckCollision(Capsule* c)  override;


		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 中心座標
			LWP::Math::Vector3 position;
			// 半径
			float radius;

			// コンストラクタ
			Data(Sphere* sphere) {
				position = sphere->position;
				radius = sphere->radius;
			}
		};
	};
};