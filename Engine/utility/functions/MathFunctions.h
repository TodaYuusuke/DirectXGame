#pragma once
#include "math/vector/Vector3.h"

namespace LWP::Utility {
	/// <summary>
	/// radian から degreeへ
	/// </summary>
	float RadianToDegree(float radian);
	/// <summary>
	/// degree から radian へ
	/// </summary>
	float DegreeToRadian(float degree);
	/// <summary>
	/// 二つの方向ベクトル間のラジアンを求める関数
	/// </summary>
	/// <param name="vec1"></param>
	/// <param name="vec2"></param>
	/// <param name="axis"></param>
	/// <returns></returns>
	float GetRadian(const Math::Vector3& vec1, const Math::Vector3& vec2, Math::Vector3 axis = Math::Vector3::UnitY());

	/// <summary>
	/// 指定の範囲内に値を収める関数
	/// </summary>
	/// <param name="value"></param>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	float ClampOnRange(float value, float min, float max);

	/// <summary>
	/// 任意の型Tのn個のデータから平均を求めて返す関数
	/// </summary>
	/// <typeparam name="T">任意の型</typeparam>
	/// <param name="data">データの配列</param>
	/// <param name="n">個数</param>
	/// <returns>平均値</returns>
	template<typename T>
	T CalculateAverage(const T* data, int n) {
		T sum = data[0];
		for (int i = 1; i < n; ++i)
		{
			sum += data[i];
		}

		return static_cast<T>(n) / sum;
	}
};

