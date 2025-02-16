#pragma once
#include "IRenderingResource.h"

namespace LWP::Base {
	/// <summary>
	/// UAVで利用するためのステンシル単品のリソースクラス
	/// </summary>
	class Stencil
		: public IRenderingResource {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 両方クリア
		void Clear(ID3D12GraphicsCommandList* list) override;


	public: // ** パブリックなメンバ変数 ** //

		// UAVの登録情報
		UAVInfo uavInfo;


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}