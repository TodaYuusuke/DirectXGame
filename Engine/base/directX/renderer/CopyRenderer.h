#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/utility/DXC.h"

namespace LWP::Base {
	/// <summary>
	/// 通常描画のレンダラー
	/// </summary>
	class CopyRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		// 通常描画のターゲット構造体
		struct Target {
			RenderResource* src;	// BackBuffer（RenderRsource）
			RenderResource* dst;	// BackBuffer（RenderRsource）
		};


		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		CopyRenderer() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~CopyRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// コピー命令実行
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		void Execute(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		/// <param name="target"></param>
		void AddTarget(const Target& target) { target_.push_back(target); }

		/// <summary>
		/// インデックスデータリセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// ターゲット配列
		std::vector<Target> target_;
	};
}