#pragma once
#include "../math/Vector4.h"

namespace LWP::Utility {
	// *** 基本色 *** //
	enum ColorPattern : unsigned int {
		WHITE = 0xFFFFFFFF,
		RED = 0xFF0000FF,
		GREEN = 0x00FF00FF,
		BLUE = 0x0000FFFF,
		YELLOW = 0xFFFF00FF,
		MAGENTA = 0xFF00FFFF,
		CYAN = 0x00FFFFFF,
		BLACK = 0x000000FF,
	};


	class Color {
	public:
		Color();
		Color(int r, int g, int b, int a);
		Color(unsigned int c);
		Color(Math::Vector4 v);

		/// <summary>
		/// 色情報（0～255）
		/// </summary>
		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;

		/// <summary>
		/// Vector4型の色情報を受け取る（0.0f～1.0f）
		/// </summary>
		/// <returns>Vector4型の色情報</returns>
		Math::Vector4 GetVector4();
	};
}