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

		void SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
			rtv_ = rtv;
			dsv_ = dsv;
			srv_ = srv;
		}

	protected: // ** メンバ変数 ** //
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list_;

		// ディスクリプタヒープのポインタ
		RTV* rtv_ = nullptr;
		DSV* dsv_ = nullptr;
		SRV* srv_ = nullptr;

	public: // ** getter ** //
		// CommandListを受け取る関数
		ID3D12GraphicsCommandList* GetList() const { return list_.Get(); }
	};
}