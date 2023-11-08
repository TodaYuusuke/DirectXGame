#pragma once
#include "../IObject.h"

namespace LWP::Object {
	class Camera : public IObject {
	public: // メンバ変数

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;

		// ビュープロジェクションを渡す関数
		Math::Matrix4x4 GetViewProjectionMatrix3D() const;
		Math::Matrix4x4 GetViewProjectionMatrix2D() const;

	private: // メンバ変数

		// PrimitiveManagerのCBufferに渡す、3D用のワールドビュープロジェクション行列
		Math::Matrix4x4 viewProjectionMatrix3D;
		// 2D用のワールドビュープロジェクション行列
		Math::Matrix4x4 viewProjectionMatrix2D;
	};
}