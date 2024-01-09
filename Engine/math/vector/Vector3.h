#pragma once
#include <cmath>
#include <vector>

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

		/// <summary>
		/// 3次元ベクトルの長さを求める
		/// </summary>
		/// <param name="v">... ベクトル</param>
		/// <returns>ベクトルの長さ</returns>
		float Length();

		/// <summary>
		/// 3次元ベクトルの正規化を求める
		/// </summary>
		/// <param name="v">... ベクトル</param>
		/// <returns>正規化されたベクトル</returns>
		Vector3 Normalize();


		// *** 静的なメンバ関数 *** //

		/// <summary>
		/// 3次元ベクトルの内積を求める
		/// </summary>
		/// <param name="v1">... ベクトル1</param>
		/// <param name="v2">... ベクトル2</param>
		/// <returns>内積</returns>
		static float Dot(const Vector3& v1, const Vector3& v2);

		/// <summary>
		/// クロス積を求める
		/// </summary>
		/// <param name="v1">... ベクトル１</param>
		/// <param name="v2">... ベクトル１</param>
		/// <returns>クロス積</returns>
		static Vector3 Cross(const Vector3& v1, const Vector3& v2);


		// 球面線形補間
		static Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {
			return v1 * (1 - t) + v2 * t;
		}


		// Catmull-Romスプライン曲線補間
		static Vector3 CatmullRom(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t);
		// Catmull-Romスプライン曲線補間の座標を求める
		static Vector3 GetCatmullRomPosition(std::vector<Vector3> controlPoints, float t);
	};
}