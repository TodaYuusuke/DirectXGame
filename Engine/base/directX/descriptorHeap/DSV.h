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
		void Initialize(ID3D12Device* device, int32_t width, int32_t height, SRV* srv);

		/// <summary>
		/// 深度をクリアする関数
		/// </summary>
		void ClearDepth(UINT index, ID3D12GraphicsCommandList* commandList);

		/// <summary>
		/// シャドウマップのリソースを返す関数
		/// </summary>
		ID3D12Resource* GetShadowMapResource() { return shadowMapResource_.Get(); }

		/// <summary>
		/// シャドウマップのビューを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetShadowView() { return shadowView_; }

	private: // ** メンバ変数 ** //
		// DepthStencilテクスチャ
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
		// シャドウマップ用テクスチャ
		Microsoft::WRL::ComPtr<ID3D12Resource> shadowMapResource_;
		// シャドウマップのリソースへのビュー
		D3D12_GPU_DESCRIPTOR_HANDLE shadowView_;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 前後関係を保存するためのリソースを作成
		/// </summary>
		void CreateDepthStencil(int32_t width, int32_t height);

		/// <summary>
		/// シャドウマップ用のリソースを作成
		/// </summary>
		void CreateShadowMap(SRV* srv);
	};
}