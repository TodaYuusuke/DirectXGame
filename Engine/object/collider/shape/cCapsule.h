#pragma once
#include "ICollider.h"

#if DEMO
#include "primitive/3d/Capsule.h"
#endif

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のCapsuleクラス
	/// </summary>
	class Capsule
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 始点
		LWP::Math::Vector3 start = { 0.0f,0.0f,0.0f };
		// 終点
		LWP::Math::Vector3 end = { 0.0f,1.0f,0.0f };
		// 半径
		float radius = 1.0f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Capsule();
		Capsule(const LWP::Math::Vector3& start);
		Capsule(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end);
		Capsule(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, const float& rad);

		// 固有の更新処理
		void Update() override;
		// ImGuiの派生クラス
		void DebugGUI() override;

		// 座標を指定して生成
		void Create(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end);
		void Create(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, const float& rad);
		// 形状を返す
		Shape GetShape() override { return Shape::Capsule; }

		// Observer用（==）
		bool operator==(const Capsule& other) {
			return {
				followModel_.GetChanged() &&
				start == other.start &&
				end == other.end &&
				radius == other.radius
			};
		}

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Capsule capsuleModel;
#endif

	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(AABB* c) override;
		//bool CheckCollision(OBB* c)  override;
		bool CheckCollision(Sphere* c)  override;
		bool CheckCollision(Capsule* c)  override;


		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 始点
			LWP::Math::Vector3 start;
			// 終点
			LWP::Math::Vector3 end;
			// 半径
			float radius;

			// コンストラクタ
			Data(Capsule* cap) {
				start = cap->start;
				end = cap->end;
				radius = cap->radius;
			}
		};
	};
};