#pragma once
#include "IShadowMap.h"

namespace LWP::Base {
	/// <summary>
	/// 平行光源のシャドウマッピング用のリソースクラス
	/// </summary>
	class SM_Direction
		: public IShadowMap {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init() override;
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list) override;


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}