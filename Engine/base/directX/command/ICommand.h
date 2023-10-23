#pragma once
#include "../GPUDevice.h"

namespace LWP::Base {
	class RTV;
	class SRV;
	class DSV;

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class ICommand {
	public: // ** パブリックなメンバ関数 ** //

		/// <summary>
		/// ディスクリプタヒープのポインタをセットする関数
		/// </summary>
		void SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
			rtv_ = rtv;
			dsv_ = dsv;
			srv_ = srv;
		}

		/// <summary>
		/// CommandListを受け取る関数
		/// </summary>
		ID3D12GraphicsCommandList* GetList() const { return list_.Get(); }

	protected: // ** メンバ変数 ** //
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list_;

		// ディスクリプタヒープのポインタ
		RTV* rtv_ = nullptr;
		DSV* dsv_ = nullptr;
		SRV* srv_ = nullptr;


	protected: // ** プライベートな関数 ** //

		/// <summary>
		/// リソースバリアの実態を作る関数
		/// </summary>
		D3D12_RESOURCE_BARRIER MakeResourceBarrier(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);
	};
}