#pragma once
#include "Surface.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面
	/// </summary>
	class Billboard3D final
		: public Surface {
	public:
		// ** 関数 ** //

		// 初期化を呼び出す
		Billboard3D() { Init(); }
	};
}