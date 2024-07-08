#pragma once
#include "ICollisionShape.h"

#if DEMO
#include "primitive/3d/Capsule.h"
#endif

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のCapsuleクラス
	/// </summary>
	class Capsule
		: public ICollisionShape {
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
		// コピーコンストラクタ
		Capsule(const Capsule& other);

		// 固有の更新処理
		void Update() override;

		// 座標を指定して生成
		void Create(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end);
		void Create(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, const float& rad);
		
		// 形状を返す
		Shape GetShape() override { return Shape::Capsule; }
		// ImGuiの派生クラス
		void DebugGUI() override;


#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Capsule capsuleModel;
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