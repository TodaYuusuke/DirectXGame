#pragma once
#include "IDescriptorHeap.h"

#include <vector>

namespace LWP::Base {
	class SRV;

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class DSV : public IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device, SRV* srv);

		/// <summary>
		/// 深度をクリアする関数
		/// </summary>
		void ClearDepth(UINT index, ID3D12GraphicsCommandList* commandList);

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
		/// 前後関係を保存するためのリソースを作成
		/// </summary>
		ID3D12Resource* CreateDepthStencilResource(int32_t width, int32_t height);
		/// <summary>
		/// 前後関係を保存するためのリソースを作成
		/// </summary>
		uint32_t CreateDepthStencil(ID3D12Resource* resource);

		/// <summary>
		/// 平行光源シャドウマップ用のリソースを作成
		/// </summary>
		ID3D12Resource* CreateDirectionShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view);
		
		/// <summary>
		/// 点光源シャドウマップ用のリソースを作成
		/// </summary>
		ID3D12Resource* CreatePointShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view);


	private: // ** メンバ変数 ** //
		// シャドウマップ用にSRVのポインタを保持する
		SRV* srv_ = nullptr;

		// スワップチェーン用の深度マップ
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffersDepthMap_;
		uint32_t backBuffersDepthIndex_;
		D3D12_CPU_DESCRIPTOR_HANDLE backBuffersDepthView_;
	};
}