#pragma once
#include "IDescriptorHeap.h"

namespace LWP::Base {
	/// <summary>
	/// RenderTargetView
	/// </summary>
	class RTV {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();


	private: // ** メンバ変数 ** //

		// Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
		// Desc
		D3D12_RENDER_TARGET_VIEW_DESC desc_;
	};
}