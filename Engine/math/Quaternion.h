#pragma once
#include "vector/Vector3.h"
#include "matrix/Matrix4x4.h"
#include <cmath>

namespace LWP::Math {

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

		Quaternion() = default;
		// Vector3のコンストラクタ
		Quaternion(const Vector3& other);
		Quaternion(float x, float y, float z, float w);

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
		/// 逆クォータニオン
		/// </summary>
		Quaternion Inverse() const;


	public: // ** 生成系関数 ** //

		/// <summary>
		/// 指定された軸周りの指定された角度に基づいてクォータニオンを生成する
		/// </summary>
		/// <param name="axis">軸周り</param>
		/// <param name="radian">角度(ラジアン)</param>
		/// <returns></returns>
		static Quaternion CreateFromAxisAngle(const Vector3& axis, float radian);
		/// <summary>
		/// ある方向からある方向への回転
		/// </summary>
		/// <returns></returns>
		static Quaternion DirectionToDirection(const Vector3& from, const Vector3& to);

		/// <summary>
		/// 内積を求める
		/// </summary>
		static float Dot(const Quaternion& v1, const Quaternion& v2);
		
		/// <summary>
		/// ある方向ベクトルのほうを向くクォータニオンを生成する
		/// </summary>
		static Quaternion ConvertDirection(const Vector3& dir);
		/// <summary>
		/// 方向ベクトルのfromからtoへのクォータニオン（動作未確認）
		/// </summary>
		static Quaternion ConvertFromTo(const Vector3& from, const Vector3& to);
		/// <summary>
		/// 回転行列からクォータニオン
		/// </summary>
		static Quaternion ConvertRotateMatrix(const Matrix4x4& rotateMatrix);
		/// <summary>
		/// オイラー角からクォータニオン
		/// </summary>
		static Quaternion ConvertEuler(const Vector3& eulerAngle);
		/// <summary>
		/// クォータニオンからオイラー角
		/// </summary>
		static Vector3 ConvertQuaternion(const Quaternion& qua);

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
		friend Quaternion operator*(float scalar, const Quaternion& vec) { return vec * scalar; }
		// Quaternion Division(/) Quaternion
		Quaternion operator/ (const Quaternion& other) const;
		Quaternion operator/= (const Quaternion& other);

		// Quaternion Equal(=) Vector3
		Quaternion operator=(const Vector3& other);


		// Observerクラス用のオペレーターオーバーロード
		bool operator==(const Quaternion& other) const;
	};
}