#pragma once
#include "IRenderingResource.h"

namespace LWP::Base {
	/// <summary>
	/// バックバッファー用のリソースクラス
	/// </summary>
	class BackBuffer
		: public IRenderingResource {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// 継承のために宣言だけしておく
		/// </summary>
		BackBuffer() = default;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="swapChain">スワップチェーンのポインタ</param>
		/// <param name="index">スワップチェーン上のインデックス</param>
		BackBuffer(IDXGISwapChain4* swapChain, int index);


		// 初期化
		void Init() override;
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;

	public: // ** パブリックなメンバ変数 ** //
			
		// RTVの登録情報
		RTVInfo rtvInfo;


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}