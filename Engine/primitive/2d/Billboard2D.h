#pragma once
#include "Surface.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面
	/// </summary>
	class Billboard2D final
		: public Surface {
	public:
		// ** 関数 ** //

		// 初期化を呼び出す
		Billboard2D() { Init(); }

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(Base::CommandManager* manager) override;
	};
}