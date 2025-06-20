#pragma once
#include "../IRenderPass.h"

namespace LWP::Base {
	/// <summary>
	/// 主にMeshShaderやPixelShaderで描画の処理を行うための基底クラス
	/// </summary>
	class IGraphicsPass
		: public IRenderPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		IGraphicsPass() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IGraphicsPass() = default;
	};
}