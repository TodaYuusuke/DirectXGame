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
		LWP::Math::Vector3 end = { 0.0f,1.0f,0.0f };
		// 半径
		float radius = 1.0f;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Capsule();
		Capsule(const LWP::Math::Vector3& start);
		Capsule(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end);
		Capsule(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, const float& rad);

		// 座標を指定して生成
		void Create(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end);
		void Create(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, const float& rad);
		// 形状を返す
		Shape GetShape() override { return Shape::Capsule; }

		// Observer用（==）
		bool operator==(const Capsule& other) {
			return {
				follow_.GetChanged() &&
				start == other.start &&
				end == other.end &&
				radius == other.radius
			};
		}

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Capsule* capsuleModel = nullptr;
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


	// データ構造体
	struct Capsule_Data {
		// 始点
		LWP::Math::Vector3 start;
		// 終点
		LWP::Math::Vector3 end;
		// 半径
		float radius;

		// コンストラクタ
		Capsule_Data(const Capsule& cap) {
			start = cap.start;
			end = cap.end;
			radius = cap.radius;
		}
	};
};