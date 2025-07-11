#pragma once
#include "IPlane.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常の描画用の機能クラス
	/// </summary>
	class INormal
		: virtual public IPlane {
	public: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		INormal();
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~INormal() = default;

		/// <summary>
		/// テクスチャと座標を一致させる関数
		/// </summary>
		/// <param name="fileName"></param>
		void FitToTexture();
	};
}