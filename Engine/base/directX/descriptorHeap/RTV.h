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
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// レンダーターゲットをクリアする関数
		/// </summary>
		void ClearRenderTarget(UINT backBufferIndex, ID3D12GraphicsCommandList* commandList);

	private: // ** メンバ変数 ** //

		// Desc
		D3D12_RENDER_TARGET_VIEW_DESC desc_;

	public: // ** getter ** //
		// Descを受け取る関数
		D3D12_RENDER_TARGET_VIEW_DESC GetDesc() const { return desc_; }

		// ハンドルを受け取る関数
		//D3D12_CPU_DESCRIPTOR_HANDLE GetHandle(int index) const;
	};
}