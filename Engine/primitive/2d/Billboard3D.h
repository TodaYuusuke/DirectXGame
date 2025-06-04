#pragma once
#include "Billboard2D.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面
	/// </summary>
	class Billboard3D final
		: public Billboard2D {
	public:	// ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Billboard3D() {	Init(); }
	};
}