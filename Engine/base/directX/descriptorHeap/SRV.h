#pragma once
#include "IDescriptorHeap.h"

#include <vector>

namespace LWP::Base {
	/// <summary>
	/// RenderTargetView
	/// </summary>
	class SRV : public IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// Viewをセット
		/// </summary>
		void SetView(D3D12_GPU_DESCRIPTOR_HANDLE view) { view_ = view; }



	private: // ** メンバ変数 ** //

		// テクスチャの一番最初のview
		D3D12_GPU_DESCRIPTOR_HANDLE view_;

	};
}