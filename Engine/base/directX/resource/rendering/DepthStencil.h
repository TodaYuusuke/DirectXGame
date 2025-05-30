#pragma once
#include "IRenderingResource.h"

namespace LWP::Base {
	/// <summary>
	/// デプスステンシルのリソースクラス
	/// </summary>
	class DepthStencil
		: public IRenderingResource {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// GPUコライダー初期化
		void InitGPUCollider();
		// 両方クリア
		void Clear(ID3D12GraphicsCommandList* list) override;
		// デプスクリア
		void ClearDepth(ID3D12GraphicsCommandList* list);
		// ステンシルクリア
		void ClearStencil(ID3D12GraphicsCommandList* list);

	public: // ** パブリックなメンバ変数 ** //

		// SRVの登録情報
		SRVInfo srvInfo;
		// DSVの登録情報
		DSVInfo dsvInfo;


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}