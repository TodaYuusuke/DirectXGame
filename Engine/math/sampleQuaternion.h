#pragma once
#include "vector/Vector3.h"
#include "matrix/Matrix4x4.h"
#include <cmath>

namespace LWP::Math {
	class Quaternion final {
	public: // メンバ関数

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Quaternion();

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="v">オイラー角</param>
		Quaternion(const Vector3& v);

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="x">x軸初期値</param>
		/// <param name="y">y軸初期値</param>
		/// <param name="z">z軸初期値</param>
		/// <param name="w">虚部初期値</param>
		Quaternion(float x, float y, float z, float w) noexcept;

	public: // 演算子オーバーロード

		inline Vector3& vec();
		inline const Vector3& vec() const;

		/// <summary>
		/// 演算子のオーバーロード(+)
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>積</returns>
		Quaternion operator+ (const Quaternion q) const;

		/// <summary>
		/// 演算子のオーバーロード(*)
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>積</returns>
		Quaternion operator* (const Quaternion q) const;

		/// <summary>
		/// 演算子のオーバーロード(*)
		/// </summary>
		/// <param name="f">float</param>
		/// <returns>積</returns>
		Quaternion operator* (const float f) const;


		/// <summary>
		/// 演算子のオーバーロード(/)
		/// </summary>
		/// <param name="q">クォータニオン</param>
		/// <returns>除算</returns>
		Quaternion operator/ (const Quaternion q) const;

		// Observerクラス用のオペレーターオーバーロード
		bool operator==(const Quaternion& other) const;

	public: // 静的なメンバ関数

		/// <summary>
		/// 共役
		/// </summary>
		/// <param name="q">変換するクォータニオン</param>
		/// <returns>共役</returns>
		static Quaternion Conjugate(const Quaternion& q);

		/// <summary>
		/// クォータニオンの長さを求める
		/// </summary>
		/// <param name="q">長さを求めるクォータニオン</param>
		/// <returns>クォータニオンの長さ</returns>
		static float Length(const Quaternion& q);
		/// <summary>
		/// クォータニオンの正規化
		/// </summary>
		/// <param name="q">正規化するクォータニオン</param>
		/// <returns>正規化されたクォータニオン</returns>
		static Quaternion Normalize(const Quaternion& q);

		/// <summary>
		/// クォータニオンの内積
		/// </summary>
		/// <param name="q1">クォータニオン1</param>
		/// <param name="q2">クォータニオン2</param>
		/// <returns>内積</returns>
		static float Dot(const Quaternion& q1, const Quaternion& q2);
		/// <summary>
		/// 逆クォータニオン生成関数
		/// </summary>
		/// <param name="q">逆にするクォータニオン</param>
		/// <returns>逆クォータニオン</returns>
		static Quaternion Inverse(const Quaternion& q);

		/// <summary>
		/// 任意軸回転を表すクォータニオン生成関数
		/// </summary>
		/// <param name="v">回転軸</param>
		/// <param name="angle">回転角</param>
		/// <returns>任意軸回転を表すクォータニオン</returns>
		static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& v, float angle);
		/// <summary>
		/// ベクトルをクォータニオンで回転させた後のベクトルを求める関数
		/// </summary>
		/// <param name="v">始点ベクトル</param>
		/// <param name="q">回転量</param>
		/// <returns>回転ベクトル</returns>
		static Vector3 RoatateVector(const Vector3& v, const Quaternion& q);

		/// <summary>
		/// クォータニオンを行列に変換する関数
		/// </summary>
		/// <param name="q">返還するクォータニオン</param>
		/// <returns>行列</returns>
		static Matrix4x4 QuaternionToMatrix(const Quaternion& q);

		/// <summary>
		/// 球面補間関数(Quaternion)
		/// </summary>
		/// <param name="t">現在のt</param>
		/// <param name="start">開始角度</param>
		/// <param name="end">終端角度</param>
		/// <returns>補完されたQuaternion</returns>
		static Quaternion Slerp(float t, const Quaternion& start, const Quaternion& end);

	public: // メンバ変数

		// 実部
		float x;
		float y;
		float z;
		// 虚部
		float w;
	};
}