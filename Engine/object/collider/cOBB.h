#pragma once
#include "cAABB.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のOBBクラス
	/// </summary>
	class OBB
		: public AABB {
	public: // ** メンバ関数 ** //
		// コンストラクタ
		OBB();

		// 形状から包み込む最小のOBBを生成する関数
		void CreateFromPrimitive(LWP::Primitive::IPrimitive* primitive) override;
		// 形状を返す
		Shape GetShape() override { return Shape::OBB; }

		// Observer用のオペレーターオーバーロード
		bool operator==(const OBB& other) {
			return {
				follow_.GetChanged() &&
				min == other.min &&
				max == other.max
			};
		}

#if DEMO
	public:
		// デバッグ用の描画関数
		void ShowWireFrame() override;
#endif
	private: // ** プライベートなメンバ変数 ** //
		// 追従する回転行列
		LWP::Math::Matrix4x4 rotateMatrix;

	private: // ** プライベートな関数 ** //
		// 更新時に形状を追従するための処理
		void UpdateShape() override;
	};
};