#pragma once
#include "../IObject.h"

namespace LWP::Object {
	class Camera final : public IObject {
	public: // ** パブリックなメンバ変数 ** //

		// FOV
		int fov = 90;

		// -- ポストエフェクトフラグ -- //

		// ブルームをかける
		bool enableBloom = false;

	public: // ** メンバ関数 ** //

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;

		// ビュープロジェクションを返す関数
		Math::Matrix4x4 GetViewProjectionMatrix3D() const;
		Math::Matrix4x4 GetViewProjectionMatrix2D() const;

	private: // メンバ変数

		// PrimitiveManagerのCBufferに渡す、3D用のワールドビュープロジェクション行列
		Math::Matrix4x4 viewProjectionMatrix3D;
		// 2D用のワールドビュープロジェクション行列
		Math::Matrix4x4 viewProjectionMatrix2D;
	};
}