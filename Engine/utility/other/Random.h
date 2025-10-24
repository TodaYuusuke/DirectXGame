#pragma once
#include "math/vector/Vector2.h"
#include "math/vector/Vector3.h"
#include <random>

namespace LWP::Utility::Random {
	/// <summary>
	/// 最小値から最大値の間のint型の値をランダムに生成する関数
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	int GenerateInt(int min, int max);
	/// <summary>
	/// 最小値から最大値の間のfloat型の値をランダムに生成する関数
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	float GenerateFloat(float min, float max);
	/// <summary>
	/// 最小値から最大値の間のfloat型の値をランダムに生成する関数
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Math::Vector2 GenerateVector2(Math::Vector2 min, Math::Vector2 max);
	/// <summary>
	/// 最小値から最大値の間のfloat型の値をランダムに生成する関数
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Math::Vector3 GenerateVector3(Math::Vector3 min, Math::Vector3 max);

	/// <summary>
	/// 乱数生成器
	/// </summary>
	inline std::random_device gRandomDevice;
	inline std::mt19937 gRandomEngine(gRandomDevice());
};

