#pragma once
#include "vector/Vector2.h"
#include "vector/Vector3.h"

namespace LWP::Math {
	/// <summary>
	/// 四角形（2Dバージョン）
	/// </summary>
	class Rectangle2D final {
	public: // ** メンバ変数 ** //
		// 最小値
		Vector2 min;
		// 最大値
		Vector2 max;
	};

	/// <summary>
	/// 四角形（3Dバージョン）
	/// </summary>
	class Rectangle3D final {
	public: // ** メンバ変数 ** //
		// 最小値
		Vector3 min;
		// 最大値
		Vector3 max;
	};
}