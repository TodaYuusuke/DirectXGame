#pragma once
#include "../IRenderingResource.h"

namespace LWP::Base {
	/// <summary>
	/// 平行光源のシャドウマッピング用のリソースクラス
	/// </summary>
	class IShadowMap
		: public IRenderingResource {
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