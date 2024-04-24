#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
namespace LWP::Math {
	class Vector4 final {
	public:
		float x;
		float y;
		float z;
		float w;

		// *** オペーレーターオーバーロード *** //

		// Vector4 Add(+) Vector4
		Vector4 operator+(const Vector4& other) const;
		Vector4& operator+=(const Vector4& other);
		// Vector4 Subtract(-) Vector4
		Vector4 operator-(const Vector4& other) const;
		Vector4& operator-=(const Vector4& other);
		// Vector4 Multiply(*) float
		Vector4 operator*(const float& other) const;
		Vector4& operator*=(const float& other);
		friend Vector4 operator*(float scalar, const Vector4& vec) { return vec * scalar; }
	};
}