#pragma once
#include "IDescriptorHeap.h"
#include "../resource/rendering/BackBuffer.h"
#include "utility/IndexManager.h"
#include "utility/Color.h"

#include <vector>

namespace LWP::Base {
	struct RTVInfo : public HeapInfo {
		D3D12_RENDER_TARGET_VIEW_DESC desc;
	};

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class RTV : public IDescriptorHeap {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		RTV() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		RTV(ID3D12Device* device);

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// RenderTargetViewを作成する関数
		/// </summary>
		RTVInfo CreateRenderTargetView(ID3D12Resource* resource);


	public: // ** getter ** //
		// Descを受け取る関数
		D3D12_RENDER_TARGET_VIEW_DESC GetDesc() const { return desc_; }

		// 現在のバッファーのインデックスを受け取る関数
		UINT GetBackBufferIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }
		// バッファーを受け取る関数
		ID3D12Resource* GetBackBuffer() { return backBuffers_[swapChain_->GetCurrentBackBufferIndex()]; }
	};
}