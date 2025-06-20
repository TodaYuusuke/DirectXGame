#pragma once
#include "../IRenderPass.h"

namespace LWP::Base {
	/// <summary>
	/// 主にComputeShaderで描画以外の計算などの処理を行うための基底クラス
	/// </summary>
	class IComputePass
		: public IRenderPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		IComputePass() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IComputePass() = default;
	};
}