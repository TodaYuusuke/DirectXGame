#pragma once
#include <cmath>

/// <summary>
/// 2次元ベクトル
/// </summary>
namespace LWP::Math {
	class Matrix4x4;

	class Vector2 final {
	public:
		float x;
		float y;

		// *** オペーレーターオーバーロード *** //

		// Vector2 Add(+) Vector2
		Vector2 operator+(const Vector2& other) const;
		Vector2& operator+=(const Vector2& other);
		// Vector2 Subtract(-) Vector2
		Vector2 operator-(const Vector2& other) const;
		Vector2& operator-=(const Vector2& other);

		// Vector2 Multiply(*) float
		Vector2 operator*(const float& other) const;
		Vector2& operator*=(const float& other);
		friend Vector2 operator*(float scalar, const Vector2& vec) { return vec * scalar; }
		// Vector2 Multiply(*) Matrix4x4
		Vector2 operator*(const Matrix4x4& other) const;
		//friend Vector2 operator*(Matrix4x4 scalar, const Vector2& vec) { return vec * scalar; }


		/// <summary>
		/// 2次元ベクトルの長さを求める
		/// </summary>
		/// <param name="v">... ベクトル</param>
		/// <returns>ベクトルの長さ</returns>
		float Length();

		/// <summary>
		/// 2次元ベクトルの正規化を求める
		/// </summary>
		/// <param name="v">... ベクトル</param>
		/// <returns>正規化されたベクトル</returns>
		Vector2 Normalize();
		

		/// <summary>
		/// 指定された角度分回転させる関数
		/// </summary>
		/// <returns></returns>
		Vector2 Rotate(float radian);
	};
}
