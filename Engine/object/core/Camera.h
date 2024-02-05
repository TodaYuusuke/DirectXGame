#pragma once
#include "../IObject.h"
#include "resources/texture/RenderTexture.h"
#include "base/directX/command/postEffect/PostProcessManager.h"
#include "base/directX//command/renderer/PostProcessRenderer.h"

namespace LWP::Object {
	class Camera final : public IObject {
	public: // ** パブリックなメンバ変数 ** //

		// FOV
		int fov = 90;

		// -- ポストエフェクトフラグ -- //

		// ポストプロセスを行うかのフラグ
		bool isUsePostProcess = false;
		// シェーダー用のパス（作成後は意味をなくすのでいつか再設計）
		std::string shaderPath = "postProcess/CCTV.PS.hlsl";


	public: // ** メンバ関数 ** //

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// シェーダー作り直し関数
		void ReCreateShader();

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

		// レンダラーのポインタを返す関数
		Base::PostProcessRenderer* GetPPRenderer() { return ppRenderer_; }
		// レンダラーのポインタをセットする関数
		void SetPPRenderer(Base::PostProcessRenderer* ppRenderer) { ppRenderer_ = ppRenderer; }

	private: // ** メンバ変数 ** //

		// このカメラからのレンダリング結果を格納する変数
		Resource::RenderTexture* renderTexture_ = nullptr;
		// ポストプロセス用レンダラー
		Base::PostProcessRenderer* ppRenderer_ = nullptr;
	};
}