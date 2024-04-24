#pragma once
#include <cmath>

/// <summary>
/// 3次元ベクトル
/// </summary>
namespace LWP::Math {
	class Matrix4x4;

	class Vector3 final {
	public:
		float x;
		float y;
		float z;

		// *** オペーレーターオーバーロード *** //

		// Vector3 Add(+) Vector3
		Vector3 operator+(const Vector3& other) const;
		Vector3& operator+=(const Vector3& other);
		// Vector3 Subtract(-) Vector3
		Vector3 operator-(const Vector3& other) const;
		Vector3& operator-=(const Vector3& other);

		// Vector3 Multiply(*) float
		Vector3 operator*(const float& other) const;
		Vector3& operator*=(const float& other);
		friend Vector3 operator*(float scalar, const Vector3& vec) { return vec * scalar; }
		// Vector3 Division(/) float
		Vector3 operator/(const float& other) const;
		Vector3& operator/=(const float& other);
		friend Vector3 operator/(float scalar, const Vector3& vec) { return vec / scalar; }
		// Vector3 Multiply(*) Matrix4x4
		Vector3 operator*(const Matrix4x4& other) const;
		//friend Vector3 operator*(Matrix4x4 scalar, const Vector3& vec) { return vec * scalar; }

		// Vector3 == Vector3
		bool operator==(const Vector3& other) const {
			return { x == other.x && y == other.y && z == other.z };
		}

		/// <summary>
		/// 3次元ベクトルの長さを求める
		/// </summary>
		/// <param name="v">... ベクトル</param>
		/// <returns>ベクトルの長さ</returns>
		[[nodiscard]] float Length() const;

		/// <summary>
		/// 3次元ベクトルの正規化を求める
		/// </summary>
		/// <param name="v">... ベクトル</param>
		/// <returns>正規化されたベクトル</returns>
		[[nodiscard]] Vector3 Normalize() const;


		// *** 静的なメンバ関数 *** //

		/// <summary>
		/// 3次元ベクトルの内積を求める
		/// </summary>
		/// <param name="v1">... ベクトル1</param>
		/// <param name="v2">... ベクトル2</param>
		/// <returns>内積</returns>
		[[nodiscard]] static float Dot(const Vector3& v1, const Vector3& v2);

		/// <summary>
		/// クロス積を求める
		/// </summary>
		/// <param name="v1">... ベクトル１</param>
		/// <param name="v2">... ベクトル２</param>
		/// <returns>クロス積</returns>
		[[nodiscard]] static Vector3 Cross(const Vector3& v1, const Vector3& v2);


		/// <summary>
		/// 二つのベクトル間の距離を求める
		/// </summary>
		/// <param name="v1">... ベクトル１</param>
		/// <param name="v2">... ベクトル２</param>
		/// <returns></returns>
		[[nodiscard]] static float Distance(const Vector3& v1, const Vector3& v2);

		// 線形補完
		[[nodiscard]] static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
		// 球面線形補間
		[[nodiscard]] static Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
	};
}