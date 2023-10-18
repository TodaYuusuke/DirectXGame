#pragma once
#include "../GPUDevice.h"

namespace LWP::Base {
	class RTV;
	class SRV;
	class DSV;

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class Command {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw(D3D12_RESOURCE_BARRIER barrier, UINT backBufferIndex, RTV* rtv, DSV* dsv, SRV* srv);

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw(D3D12_RESOURCE_BARRIER barrier);

		/// <summary>
		/// リセット用関数
		/// </summary>
		void Reset();


	private: // ** メンバ変数 ** //
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list_;


	public: // ** getter ** //
		// CommandListを受け取る関数
		ID3D12GraphicsCommandList* GetList() const { return list_.Get(); }
	};
}