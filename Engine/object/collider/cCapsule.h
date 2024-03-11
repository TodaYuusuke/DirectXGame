#pragma once
#include "ICollider.h"

// 前方宣言
namespace LWP::Primitive {
	class Capsule;
}

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
		LWP::Math::Vector3 end = { 0.0f,0.0f,0.0f };
		// 半径
		float radius = 1.0f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Capsule();
		Capsule(const LWP::Math::Vector3& pos);
		Capsule(const float& rad);
		Capsule(const LWP::Math::Vector3& pos, const float& rad);

		// 座標を指定して生成
		void Create(const LWP::Math::Vector3& pos);
		void Create(const LWP::Math::Vector3& pos, const float& rad);
		// 形状から包み込む最小のAABBを生成する関数
		void CreateFromPrimitive(LWP::Primitive::IPrimitive* primitive);
		// 形状を返す
		Shape GetShape() override { return Shape::Sphere; }

		// Observer用（==）
		bool operator==(const Sphere& other) {
			return {
				follow_.GetChanged() &&
				position == other.position &&
				radius == other.radius
			};
		}

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Sphere* sphereModel = nullptr;
	public:
		// デバッグ用の描画関数
		void ShowWireFrame() override;
#endif

	private: // ** プライベートな関数 ** //
		// 更新時に形状を追従するための処理
		void UpdateShape() override;
		// ImGuiの派生クラス
		void DerivedDebugGUI() override;
	};
};