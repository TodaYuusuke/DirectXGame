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

		// ワールドビュープロジェクションのポインタを渡す関数
		Math::Matrix4x4* GetwvpMatrixPtr() { return &worldViewProjectionMatrix; }

	private: // メンバ変数

		// PrimitiveManagerのCBufferに渡すワールドビュープロジェクション行列
		Math::Matrix4x4 worldViewProjectionMatrix;
	};
}