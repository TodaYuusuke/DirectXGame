#pragma once
#include "../math/Vector4.h"

namespace LWP::Utility {
	class Color {
	public:
		Color();
		Color(int r, int g, int b, int a);

		/// <summary>
		/// 色情報（0～255）
		/// </summary>
		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;

		/// <summary>
		/// unsinged int型の色情報をVector4に変換する
		/// </summary>
		/// <returns>Vector4型の色情報</returns>
		Math::Vector4 GetVector4();
	};
}