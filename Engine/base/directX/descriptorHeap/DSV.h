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
		/// 前後関係を保存するためのリソースを作成
		/// </summary>
		uint32_t CreateDepthStencil(ID3D12Resource* resource, D3D12_CPU_DESCRIPTOR_HANDLE* view, int32_t width, int32_t height);

		/// <summary>
		/// 平行光源シャドウマップ用のリソースを作成
		/// </summary>
		ID3D12Resource* CreateDirectionShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view);
		
		/// <summary>
		/// 点光源シャドウマップ用のリソースを作成
		/// </summary>
		ID3D12Resource* CreatePointShadowMap(uint32_t* dsvIndex, D3D12_GPU_DESCRIPTOR_HANDLE* view);


		/// <summary>
		/// 使用数を+1増加
		/// </summary>
		void AddUsedCount() { usedCount_++; }
		/// <summary>
		/// 現在の使用数を取得
		/// </summary>
		uint32_t GetUsedCount() { return usedCount_; }


	private: // ** メンバ変数 ** //
		// シャドウマップ用にSRVのポインタを保持する
		SRV* srv_ = nullptr;
		// 使用済みカウント初期値0
		uint32_t usedCount_ = 0;
	};
}