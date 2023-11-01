#pragma once
#include "../GPUDevice.h"
#include "ResourceStruct.h"
#include "PSO.h"

#include <memory>

namespace LWP::Base {
	class RTV;
	class SRV;
	class DSV;

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class ICommand {
	public: // ** パブリックなメンバ変数 ** //

		// インデックス情報の構造体
		std::unique_ptr<IndexResourceBuffer> indexResourceBuffer_;
		const UINT kMaxIndex = 655350 * 3;

	public: // ** メンバ関数 ** //

		ICommand() = default;
		~ICommand() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature, ID3D12Resource* resource);

		/// <summary>
		/// 描画前処理
		/// </summary>
		virtual void PreDraw(ID3D12GraphicsCommandList* list) = 0;

		/// <summary>
		/// 描画語処理
		/// </summary>
		virtual void PostDraw(ID3D12GraphicsCommandList* list) = 0;

		/// <summary>
		/// ディスクリプタヒープのポインタをセットする関数
		/// </summary>
		void SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
			rtv_ = rtv;
			dsv_ = dsv;
			srv_ = srv;
		}

		/// <summary>
		/// PSOのStateを取得
		/// </summary>
		ID3D12PipelineState* GetPSOState() { return pso_->state_.Get(); }


	protected: // ** メンバ変数 ** //

		// ディスクリプタヒープのポインタ
		RTV* rtv_ = nullptr;
		DSV* dsv_ = nullptr;
		SRV* srv_ = nullptr;

		// グラフィックパイプラインの状態を定義
		std::unique_ptr<PSO> pso_;


	protected: // ** プライベートな関数 ** //

		/// <summary>
		/// PSOを作成
		/// </summary>
		virtual void CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature);

		/// <summary>
		/// リソースバリアの実態を作る関数
		/// </summary>
		D3D12_RESOURCE_BARRIER MakeResourceBarrier(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);
	};
}