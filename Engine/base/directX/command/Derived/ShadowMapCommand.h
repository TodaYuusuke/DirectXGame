#pragma once
#include "../ICommand.h"

namespace LWP::Base {
	/// <summary>
	/// コマンドクラス
	/// </summary>
	class ShadowMapCommand : public ICommand {
	public:	// ** メンバ関数 ** //

		/// <summary>
		/// レンダリング先のデータをセット
		/// </summary>
		void SetDrawTarget(const Math::Matrix4x4& vp, ID3D12Resource* resource, uint32_t dsvIndex);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw(ID3D12GraphicsCommandList* list) override;

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw(ID3D12GraphicsCommandList* list) override;


	private: // ** メンバ変数 ** // 

		ID3D12Resource* resource_ = nullptr;	// 書き込み先のリソース
		uint32_t dsvIndex_;		// 書き込み先のDSVインデックス


	private: // ** プライベートな関数 ** //

		/// <summary>
		/// PSOを作成
		/// </summary>
		void CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) override;
	};
}