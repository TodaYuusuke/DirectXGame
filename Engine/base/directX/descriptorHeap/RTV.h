#pragma once
#include "IDescriptorHeap.h"

#include <vector>

namespace LWP::Base {
	/// <summary>
	/// RenderTargetView
	/// </summary>
	class RTV : public IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(HWND hwnd, GPUDevice* gpuDevice, int32_t width, int32_t height, ID3D12CommandQueue* queue);

		/// <summary>
		/// レンダーターゲットをクリアする関数
		/// </summary>
		void ClearRenderTarget(ID3D12GraphicsCommandList* commandList);

	private: // ** メンバ変数 ** //

		// スワップチェーン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
		// Desc
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
		// バッファー配列
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;

		// Desc
		D3D12_RENDER_TARGET_VIEW_DESC desc_;

	public: // ** getter ** //
		// Descを受け取る関数
		D3D12_RENDER_TARGET_VIEW_DESC GetDesc() const { return desc_; }

		// スワップチェーンを受け取る関数
		IDXGISwapChain4* GetSwapChain() const { return swapChain_.Get(); }
		// スワップチェーンのDescを受け取る関数
		DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }
		// 現在のバッファーのインデックスを受け取る関数
		UINT GetBackBufferIndex() const { return swapChain_->GetCurrentBackBufferIndex(); }
		// バッファーを受け取る関数
		ID3D12Resource* GetBackBuffer() const { return backBuffers_[swapChain_->GetCurrentBackBufferIndex()].Get(); }
	};
}