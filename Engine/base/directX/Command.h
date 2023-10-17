#pragma once
#include "GPUDevice.h"

namespace LWP::Base {
	/// <summary>
	/// RenderTargetView
	/// </summary>
	class Command {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();


	private: // ** メンバ変数 ** //
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list_;
	};
}