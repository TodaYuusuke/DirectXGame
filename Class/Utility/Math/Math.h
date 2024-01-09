#pragma once
#include <cstdlib>

namespace Math {
	// ランダムなfloat値を返す関数
	float RandomFloat(float min, float max) {
		// 0.0から1.0の範囲の乱数を生成
		float random01 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		// 指定された範囲にスケーリングして返す
		return min + random01 * (max - min);
	}
};