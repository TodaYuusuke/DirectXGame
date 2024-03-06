#pragma once
#include "ICollider.h"

// 前方宣言
namespace LWP::Primitive {
	class IPrimitive;
	class Cube;
}

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のAABBクラス
	/// </summary>
	class AABB 
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 最小
		LWP::Math::Vector3 min = { -0.5f,-0.5f,-0.5f };
		// 最大
		LWP::Math::Vector3 max = { 0.5f,0.5f,0.5f };


	public: // ** メンバ関数 ** //
		// コンストラクタ
		AABB();
		AABB(const LWP::Math::Vector3& min, const LWP::Math::Vector3& max);

		// 形状から包み込む最小のAABBを生成する関数
		void CreateFromPrimitive(LWP::Primitive::IPrimitive* primitive);
		// 形状を返す
		Shape GetShape() override { return Shape::AABB; }

		// Observer用（==）
		bool operator==(const AABB& other) {
			return { 
				follow_.GetChanged() &&
				min == other.min &&
				max == other.max
			};
		}

#if DEMO
	private:
		// デバッグ用モデル
		LWP::Primitive::Cube* cube = nullptr;
	public:
		// デバッグ用の描画関数
		void ShowWireFrame(Base::CommandManager* manager) override;
#endif

	private: // ** プライベートな関数 ** //
		// ImGuiの派生クラス
		void DerivedDebugGUI() override;
	};
};