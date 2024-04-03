#pragma once
#include "IDescriptorHeap.h"

#include <vector>

namespace LWP::Base {
	struct DSVInfo : public HeapInfo {
		D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	};

	class SRV;

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class DSV : public IDescriptorHeap {
	public:	// ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		DSV() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		DSV(ID3D12Device* device);

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();


		/// <summary>
		/// バックバッファ用の深度マップのリソースを受け取る関数
		/// </summary>
		ID3D12Resource* GetBackBuffersDepth() { return backBuffersDepthMap_.Get(); }
		/// <summary>
		/// バックバッファ用の深度マップのIndexを受け取る関数
		/// </summary>
		uint32_t GetBackBuffersDepthIndex() { return backBuffersDepthIndex_; }
		/// <summary>
		/// バックバッファ用の深度マップのViewを受け取る関数
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetBackBuffersDepthView() { return backBuffersDepthView_; }
		
		/// <summary>
		/// デプスステンシルビューを作成
		/// </summary>
		DSVInfo CreateDepthStencilView(ID3D12Resource* resource);

		/// <summary>
		/// 平行光源シャドウマップ用のリソースを作成
		/// </summary>
		ID3D12Resource* CreateDirectionShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view);
		
		/// <summary>
		/// 点光源シャドウマップ用のリソースを作成
		/// </summary>
		ID3D12Resource* CreatePointShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view);

	};
}