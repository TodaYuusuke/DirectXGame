#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"
#include "base/directX/postProcess/PostProcessor.h"
#include "base/directX/utility/DXC.h"


namespace LWP::Base {
	/// <summary>
	/// 通常描画のレンダラー
	/// </summary>
	class PPRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		// 通常描画のターゲット構造体
		struct Target {
			RenderResource* render;		// ポストプロセスを掛けた結果を格納するリソース
			BackBuffer* texture;	// ポストプロセスを掛けるテクスチャ
			DepthStencil* depth;		// デプスのリソース
			PostProcessor* pp;			// ポストプロセス用のデータ
		};


		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PPRenderer() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~PPRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 描画命令
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		/// <param name="target">ターゲット</param>
		void DrawCall(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		/// <param name="target"></param>
		void AddTarget(const Target& target) { target_.push_back(target); }

		/// <summary>
		/// データリセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// ターゲット配列
		std::vector<Target> target_;
	};
}