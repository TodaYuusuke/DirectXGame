#pragma once
#include "../IObject.h"

namespace LWP::Object {
	class Camera : public IObject {
	public: // メンバ変数

		// 初期化
		void Initialize() override;
		// 更新
		void Update() override;
		// 描画
		void Draw() override;

		// ビュープロジェクションを渡す関数
		Math::Matrix4x4 GetViewProjectionMatrix() { return viewProjectionMatrix; }

	private: // メンバ変数

		// PrimitiveManagerのCBufferに渡すワールドビュープロジェクション行列
		Math::Matrix4x4 viewProjectionMatrix;
	};
}