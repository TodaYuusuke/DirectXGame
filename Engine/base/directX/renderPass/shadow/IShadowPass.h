#pragma once
#include "../IRenderPass.h"

namespace LWP::Base {
	/// <summary>
	/// 主にシャドウマッピングの用の描画を行うための基底クラス
	/// </summary>
	class IShadowPass
		: public IRenderPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		IShadowPass() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IShadowPass() = default;
	};
}