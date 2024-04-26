#pragma once
#include <cmath>

namespace LWP::Math {
	// 前方宣言
	class Vector3;

	/// <summary>
	/// クォータニオンクラス
	/// </summary>
	class Quaternion final {
	public: // ** パブリックなメンバ関数 ** //
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;

		// vectorと同じように扱うための処理
		inline Vector3& vec() { return *reinterpret_cast<Vector3*>(&x); }
		inline const Vector3& vec() const { return *reinterpret_cast<const Vector3*>(&x); }


	public: // ** メンバ関数 ** //
		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 正規化されたクォータニオンを求める
		/// </summary>
		/// <returns>正規化されたクォータニオン</returns>
		Quaternion Normalize() const;
		/// <summary>
		/// 
		/// </summary>
		Quaternion Conjugate() const;
		/// <summary>
		/// 長さ
		/// </summary>
		float Length() const;
		/// <summary>
		/// 
		/// </summary>
		Quaternion Inverse() const;

	public: // ** 生成系関数 ** //
		/// <summary>
		/// 二点からクォータニオンを求める
		/// <param name="start">... 始点</param>
		/// <param name="end">... 終点</param>
		/// <returns>二点の回転を表すクォータニオン</returns>
		static Quaternion CreateRotationX(const Vector3& start, const Vector3& end);
		static Quaternion CreateRotationY(const Vector3& start, const Vector3& end);
		static Quaternion CreateRotationZ(const Vector3& start, const Vector3& end);

	//private:
		static Quaternion CreateRotation(const Vector3& start, const Vector3& end, const Vector3& axis);

		/// <summary>
		/// 内積を求める
		/// </summary>
		static float Dot(const Quaternion& v1, const Quaternion& v2);

	public: // ** オペレータオーバーロード ** //

		// Quaternion Add(+) Quaternion
		Quaternion operator+ (const Quaternion other) const;
		Quaternion operator+= (const Quaternion other);
		// Quaternion Multiply(*) Quaternion
		Quaternion operator*(const Quaternion& other) const;
		Quaternion& operator*=(const Quaternion& other);
		/// Quaternion Multiply(*) float
		Quaternion operator* (const float& other) const;
		Quaternion& operator*=(const float& other);
		// Quaternion Division(/) Quaternion
		Quaternion operator/ (const Quaternion& other) const;
		Quaternion operator/= (const Quaternion& other);

		// Quaternion Equal(=) Vector3
		Quaternion operator=(const Vector3& other);
	};
}