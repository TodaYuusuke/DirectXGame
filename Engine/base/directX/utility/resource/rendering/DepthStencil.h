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
		void Init(GPUDevice* device, HeapManager* heaps) override;
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;

	public: // ** パブリックなメンバ変数 ** //

		// DSVの登録情報
		DSVInfo dsvInfo;


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}