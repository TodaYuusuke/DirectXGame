#pragma once
#include "utility/Singleton.h"

#include "graphics/DrawSolid.h"
#include "graphics/ImGuiRender.h"
#include "graphics/RenderClear.h"
#include "copy/Copy.h"

#include "resources/texture/Texture.h"

// 前方宣言
namespace LWP {
	namespace Object {
		class Camera;
	}
}

namespace LWP::Base {
	/// <summary>
	/// レンダリングの順序を管理するクラス
	/// </summary>
	class RenderingPipeline final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
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
		void DrawCall();

		/// <summary>
		/// ImGuiにバッファーカウントを渡すための関数
		/// </summary>
		/// <returns></returns>
		UINT GetBufferCount() { return swapChainDesc_.BufferCount;  }


	private: // ** メンバ変数 ** //

		// レンダリングパスの配列
		//std::vector<IRenderPass*> renderPasses_;
		// コマンド管理クラス
		Command commander_;

		// スワップチェーン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
		// Desc
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
		// バッファー配列
		std::vector<BackBuffer> backBuffers_;
		// テクスチャがないときのデフォルトテクスチャ
		Resource::Texture defaultTexture_;

		DrawSolid drawSolid_;	// Solidオブジェクトを描画するパス
		RenderClear renderClear_;	// レンダリングをクリアするパス
		Copy copy_;		// リソースをコピーするパス
		ImGuiRender imguiRender_;	// ImGuiを描画するパス

	};
}