#pragma once

#include "graphics/DrawSolid.h"

namespace LWP::Base {
	/// <summary>
	/// レンダリングの順序を管理するクラス
	/// </summary>
	class RenderingPipeline {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		RenderingPipeline() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~RenderingPipeline() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画命令を積み込み、実行する
		/// </summary>
		void Update();

	private: // ** メンバ変数 ** //

		// レンダリングパスの配列
		//std::vector<IRenderPass*> renderPasses_;

		DrawSolid drawSolid_;	// Solidオブジェクトを描画するパス
	};
}