#pragma once
#include "IPrimitive3D.h"

namespace LWP::Primitive {
	/// <summary>
	/// 円柱
	/// </summary>
	class Cylinder
		: public IPrimitive3D {
	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Cylinder();
		/// <summary>
		/// タイプを返す
		/// </summary>
		Type GetType() { return Type::Cylinder; }
	};
}