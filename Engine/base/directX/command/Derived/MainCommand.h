#pragma once
#include "../ICommand.h"

namespace LWP::Base {
	/// <summary>
	/// コマンドクラス
	/// </summary>
	class MainCommand : public ICommand {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void DerivedInitialize() override;

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw(ID3D12GraphicsCommandList* list) override;

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw(ID3D12GraphicsCommandList* list) override;


	private: // ** メンバ変数 ** // 

		struct DepthStencil {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource_;	// リソース
			D3D12_CPU_DESCRIPTOR_HANDLE view_;					// ビュー
		};
		// 前後関係用の深度マップ
		std::unique_ptr<DepthStencil> depthStencil_;


	private: // ** プライベートな関数 ** //

		/// <summary>
		/// PSOを作成
		/// </summary>
		void CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) override;
	};
}