#pragma once
#include "../utility/GPUDevice.h"

#include "../utility/Command.h"
#include "../utility/RootSignature.h"
#include "../utility/PSO.h"

namespace LWP::Base {
	/// <summary>
	/// レンダリングを組みやすくするための基底クラス
	/// </summary>
	class IRenderPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		IRenderPass() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IRenderPass() = default;
	};
}