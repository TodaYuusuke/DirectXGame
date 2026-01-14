#pragma once
#include "IPostProcessPass.h"

namespace LWP::Base {
	/// <summary>
	/// Bloomのレンダリングを行うためのパス
	/// </summary>
	class BloomRender
		: public IPostProcessPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		BloomRender() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~BloomRender() = default;
	};
}