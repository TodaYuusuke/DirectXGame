#pragma once
#include "IDescriptorHeap.h"

#include <vector>

namespace LWP::Base {
	/// <summary>
	/// RenderTargetView
	/// </summary>
	class DSV : public IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device, int32_t width, int32_t height);

		/// <summary>
		/// 深度をクリアする関数
		/// </summary>
		void ClearDepth(ID3D12GraphicsCommandList* commandList);

	private: // ** メンバ変数 ** //
		// DepthStencilテクスチャ
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	};
}