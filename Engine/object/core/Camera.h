#pragma once
#include "../IObject.h"
#include "resources/texture/RenderTexture.h"
#include "base/directX/command/postEffect/LensDistortion.h"
#include "base/directX/command/derived/SubRendering.h"

namespace LWP::Object {
	class Camera final : public IObject {
	public: // ** パブリックなメンバ変数 ** //

		// FOV
		int fov = 90;

		// -- ポストエフェクトフラグ -- //

		// レンズディストーション（歪曲収差）
		LensDistortion lensDistortion;
		// ブルームをかける
		//bool enableBloom = false;

	public: // ** メンバ関数 ** //

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;

		// ビュープロジェクションを返す関数
		Math::Matrix4x4 GetViewProjection() const;

		/// <summary>
		/// このカメラからのサブ画面レンダリング用のテクスチャを生成する関数
		/// </summary>
		/// <param name="manager"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void CreateRenderTexture(Base::DirectXCommon* directX, const int width, const int height);
		// レンダリング結果のテクスチャのポインタを受け取る関数
		Resource::RenderTexture* GetRenderTexture() { return renderTexture_; }


	private: // ** メンバ変数 ** //

		// このカメラからのレンダリング結果を格納する変数
		Resource::RenderTexture* renderTexture_ = nullptr;
	};
}