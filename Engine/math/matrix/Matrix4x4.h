#pragma once
#include <cmath>
#include <assert.h>

/// <summary>
/// 4x4行列
/// </summary>
namespace LWP::Math {
	class Vector3;

	class Matrix4x4 final {
	public:
		float m[4][4];

		// *** オペーレーターオーバーロード *** //

		// Matrix4x4 Add(+) Matrix4x4
		Matrix4x4 operator+(const Matrix4x4& other) const;

		// Matrix4x4 Subtract(-) Matrix4x4
		Matrix4x4 operator-(const Matrix4x4& other) const;

		// Matrix4x4 Multiply(*) Matrix4x4
		Matrix4x4 operator*(const Matrix4x4& other) const;
		// Matrix4x4 Multiply(*) float
		Matrix4x4 operator*(const float& other) const;
		friend Matrix4x4 operator*(float scalar, const Matrix4x4& vec) { return vec * scalar; }



		// *** 任意の行列を返す関数 *** //

		// 行列式を返す関数
		float Determinant();
		// 逆行列を返す関数
		Matrix4x4 Inverse();
		// 転置行列を返す関数
		Matrix4x4 Transpose();


		// *** 行列作成 *** //

#pragma region アフィン行列

		// 平行移動行列
		static Matrix4x4 CreateTranslateMatrix(const Vector3& translate);
		// X軸回転行列
		static Matrix4x4 CreateRotateXMatrix(const float& radian);
		// y軸回転行列
		static Matrix4x4 CreateRotateYMatrix(const float& radian);
		// z軸回転行列
		static Matrix4x4 CreateRotateZMatrix(const float& radian);
		// 合成された回転行列を返す vb、。
		static Matrix4x4 CreateRotateXYZMatrix(const Vector3& rotate);
		// 拡大縮小行列
		static Matrix4x4 CreateScaleMatrix(const Vector3& scale);

		// 3次元アフィン変換行列
		static Matrix4x4 CreateAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

#pragma endregion

#pragma region レンダリングパイプライン
		
		// 透視射影行列
		static Matrix4x4 CreatePerspectiveFovMatrix(const float& fovY, const float& aspectRatio, const float& nearClip, const float& farClip);
		// 正射影行列
		static Matrix4x4 CreateOrthographicMatrix(const float& left, const float& top, const float& right, const float& bottom, const float& nearClip, const float& farClip);
		// ビューポート変換行列
		static Matrix4x4 CreateViewportMatrix(const float& left, const float& top, const float& width, const float& height, const float& minDepth, const float& maxDepth);

		// ライトのビュー行列
		static Matrix4x4 CreateLookAtMatrix(const Vector3& eye, const Vector3& at, const Vector3& up);

#pragma endregion

		// 単位行列の作成
		static Matrix4x4 CreateIdentity4x4();

		// 座標変換
		static Vector3 TransformCoord(const Vector3& vector, const Matrix4x4& matrix);
	};
}