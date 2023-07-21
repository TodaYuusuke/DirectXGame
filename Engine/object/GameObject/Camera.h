#pragma once
#include "../IObject.h"

namespace LWP::Object {
	class Camera : public IObject {
	public: // メンバ変数

		// 初期化
		void Initialize() override;
		// 更新
		void Update() override;

	private: // メンバ変数

		// PrimitiveControllerのCBufferに渡すワールドビュープロジェクション行列
		Math::Matrix4x4 worldViewProjectionMatrix;
	};
}