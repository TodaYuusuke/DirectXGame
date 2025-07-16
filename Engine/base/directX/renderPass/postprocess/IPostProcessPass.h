#pragma once
#include "../IRenderPass.h"

namespace LWP::Base {
	/// <summary>
	/// 主にポストプロセスの処理を行うための基底クラス
	/// </summary>
	class IPostProcessPass
		: public IRenderPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		IPostProcessPass() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IPostProcessPass() = default;
	};
}