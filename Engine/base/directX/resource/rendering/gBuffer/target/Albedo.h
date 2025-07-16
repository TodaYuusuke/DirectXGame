#pragma once
#include "../../IRenderingResource.h"

namespace LWP::Base::GBufferTarget {
	/// <summary>
	/// 遅延ライティングのアルベド
	/// </summary>
	class Albedo
		: public IRenderingResource {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;


	public: // ** パブリックなメンバ変数 ** //
		
		RTVInfo rtvInfo;	// rtv情報
		SRVInfo srvInfo;	// srv情報

	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}