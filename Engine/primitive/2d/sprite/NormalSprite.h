#pragma once
#include "ISprite.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のスプライト
	/// </summary>
	class NormalSprite
		: public ISprite {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		NormalSprite();
	};
}