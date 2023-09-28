#pragma once
#include <cmath>
#include <assert.h>

/// <summary>
/// 4x4行列
/// </summary>
namespace LWP::Math {
	class Vector2;

	class Matrix3x3 final {
	public:
		float m[3][3];

		// *** オペーレーターオーバーロード *** //

		// Matrix3x3 Add(+) Matrix3x3
		Matrix3x3 operator+(const Matrix3x3& other) const;

		// Matrix3x3 Subtract(-) Matrix3x3
		Matrix3x3 operator-(const Matrix3x3& other) const;

		// Matrix3x3 Multiply(*) Matrix3x3
		Matrix3x3 operator*(const Matrix3x3& other) const;
		// Matrix3x3 Multiply(*) float
		Matrix3x3 operator*(const float& other) const;
		friend Matrix3x3 operator*(float scalar, const Matrix3x3& vec) { return vec * scalar; }


		// *** 任意の行列を返す関数 *** //

		// 行列式を返す関数
		float Determinant();
		// 逆行列を返す関数
		Matrix3x3 Inverse();
		// 転置行列を返す関数
		Matrix3x3 Transpose();


		// *** 行列作成 *** //

#pragma region アフィン行列

		// 平行移動行列
		static Matrix3x3 CreateTranslateMatrix(const Vector2& translate);
		// 回転行列
		static Matrix3x3 CreateRotateMatrix(const float& radian);
		// 拡大縮小行列
		static Matrix3x3 CreateScaleMatrix(const Vector2& scale);

		// 2次元アフィン変換行列
		static Matrix3x3 CreateAffineMatrix(const Vector2& scale, const float& rotate, const Vector2& translate);

#pragma endregion

		// 単位行列の作成
		static Matrix3x3 CreateIdentity4x4();
	};
}