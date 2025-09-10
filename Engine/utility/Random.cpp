#include "Random.h"

#include <cassert>

namespace LWP::Utility::Random {
	int GenerateInt(int min, int max) {
		assert(min <= max && "[min] is greater than [max].");

		std::uniform_int_distribution<int> dist(min, max);	// 整数の一様分布を定義
		return dist(gRandomEngine);								// 乱数を生成して返す
	}

	float GenerateFloat(float min, float max) {
		assert(min <= max && "[min] is greater than [max].");

		std::uniform_real_distribution<float> dist(min, max);	// 整数の一様分布を定義
		return dist(gRandomEngine);								// 乱数を生成して返す
	}

	Math::Vector3 GenerateVector3(Math::Vector3 min, Math::Vector3 max) {
		assert(min.x <= max.x || min.y <= max.y || min.z <= max.z && "[min] is greater than [max].");
		// 各成分ごとに乱数を生成してVector3を返す
		return Math::Vector3{
			GenerateFloat(min.x, max.x),
			GenerateFloat(min.y, max.y),
			GenerateFloat(min.z, max.z)
		};
	}
}