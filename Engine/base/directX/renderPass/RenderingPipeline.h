#pragma once
#include "utility/other/Singleton.h"

#include "graphics/DrawSolid.h"
#include "graphics/DrawPlane.h"
#include "graphics/ImGuiRender.h"
#include "graphics/RenderClear.h"
#include "graphics/DeferredShading.h"

#include "shadow/DirLightShadow.h"

#include "copy/Copy.h"
#include "postprocess/CustomShaderRender.h"

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

		RenderClear renderClear_;	// レンダリングをクリアするパス

		DrawSolid drawSolid_;	// Solidオブジェクトを描画するパス
		DrawPlane drawPlane_;	// Planeオブジェクトを描画するパス
		DeferredShading deferredShading_;	// 遅延ライティングを行うパス
		Copy copy_;		// リソースをコピーするパス
		ImGuiRender imguiRender_;	// ImGuiを描画するパス

		DirLightShadow dirShadow_;	// 平行光源のシャドウを描画するパス
		CustomShaderRender customShaderRender_;	// ポストプロセスを行うパス
	};
}