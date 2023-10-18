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
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize(ID3D12Device* device) = 0;

		/// <summary>
		/// 描画前処理
		/// </summary>
		virtual void PreDraw(D3D12_RESOURCE_BARRIER barrier, UINT backBufferIndex, RTV* rtv, DSV* dsv, SRV* srv) = 0;

		/// <summary>
		/// 描画語処理
		/// </summary>
		virtual void PostDraw(D3D12_RESOURCE_BARRIER barrier) = 0;

		/// <summary>
		/// リセット用関数
		/// </summary>
		virtual void Reset() = 0;


	protected: // ** メンバ変数 ** //
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list_;


	public: // ** getter ** //
		// CommandListを受け取る関数
		ID3D12GraphicsCommandList* GetList() const { return list_.Get(); }
	};
}