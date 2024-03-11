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


	public: // ** オペーレーターオーバーロード ** //

		// Quaternion Multiply(*) Quaternion
		Quaternion operator*(const Quaternion& other) const;
		Quaternion& operator*=(const Quaternion& other);

		// Quaternion Equal(=) Vector3
		Quaternion operator=(const Vector3& other);


	public: // ** メンバ関数 ** //
		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 正規化されたクォータニオンを求める
		/// </summary>
		/// <param name="v">... クォータニオン</param>
		/// <returns>正規化されたクォータニオン</returns>
		Quaternion Normalize();
		
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
	};
}