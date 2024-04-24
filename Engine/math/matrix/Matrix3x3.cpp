#include "Matrix3x3.h"
#include "../vector/Vector2.h"

using namespace LWP::Math;

// *** オペーレーターオーバーロード *** //

// Matrix3x3 Add(+) Matrix3x3
Matrix3x3 Matrix3x3::operator+(const Matrix3x3& other) const {
	Matrix3x3 result{};
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			result.m[y][x] = m[y][x] + other.m[y][x];
		}
	}
	return result;
}

// Matrix3x3 Subtract(-) Matrix3x3
Matrix3x3 Matrix3x3::operator-(const Matrix3x3& other) const {
	Matrix3x3 result{};
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			result.m[y][x] = m[y][x] - other.m[y][x];
		}
	}
	return result;
}

// Matrix3x3 Multiply(*) Matrix3x3
Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const {
	Matrix3x3 result{};
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			result.m[y][x] = (m[y][0] * other.m[0][x]) +
				(m[y][1] * other.m[1][x]) +
				(m[y][2] * other.m[2][x]);
		}
	}
	return result;
}
// Matrix3x3 Multiply(*) float
Matrix3x3 Matrix3x3::operator*(const float& other) const {
	Matrix3x3 result{};
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			result.m[y][x] = m[y][x] * other;
		}
	}
	return result;
}

// *** 任意の行列を返す関数 *** //

// 行列式を返す関数
float Matrix3x3::Determinant() {
	float result = 0.0f;
	for (int i = 0; i < 3; ++i) {
		result += m[0][i] * (m[1][(i + 1) % 3] * m[2][(i + 2) % 3] - m[1][(i + 2) % 3] * m[2][(i + 1) % 3]);
	}
	return result;
}
// 逆行列を返す関数
Matrix3x3 Matrix3x3::Inverse() {
	Matrix3x3 result{};

	// 行列式を計算
	float det = Determinant();

	if (det == 0.0f) {
		// 行列式が0の場合、逆行列は存在しない
		return result;
	}

	// 行列の余因子行列を計算し、逆行列を求める
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			float minorDet = m[(i + 1) % 3][(j + 1) % 3] * m[(i + 2) % 3][(j + 2) % 3] - m[(i + 1) % 3][(j + 2) % 3] * m[(i + 2) % 3][(j + 1) % 3];
			result.m[j][i] = minorDet / det;
		}
	}

	return result;
}
// 転置行列を返す関数
Matrix3x3 Matrix3x3::Transpose() {
	Matrix3x3 result{};
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			result.m[y][x] = m[x][y];
		}
	}
	return result;
}

// *** 行列作成機 *** //

// 平行移動行列
Matrix3x3 Matrix3x3::CreateTranslateMatrix(const Vector2& translate) {
	Matrix3x3 result{};
	result.m[0][0] = 1;
	result.m[1][1] = 1;
	result.m[2][2] = 1;
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	return result;
}
// 合成された回転行列を返す
Matrix3x3 Matrix3x3::CreateRotateMatrix(const float& radian) {
	Matrix3x3 result{};
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[2][2] = 1;
	return result;
}
// 拡大縮小行列
Matrix3x3 Matrix3x3::CreateScaleMatrix(const Vector2& scale) {
	Matrix3x3 result{};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = 1;
	return result;
}

// 3次元アフィン変換行列
Matrix3x3 Matrix3x3::CreateAffineMatrix(const Vector2& scale, const float& rotate, const Vector2& translate) {
	Matrix3x3 s = CreateScaleMatrix(scale);
	Matrix3x3 r = CreateRotateMatrix(rotate);
	Matrix3x3 t = CreateTranslateMatrix(translate);
	return s * r * t;
}


// 単位行列作成
Matrix3x3 Matrix3x3::CreateIdentity4x4() {
	Matrix3x3 result{};
	for (int i = 0; i < 3; i++) {
		result.m[i][i] = 1;
	}
	return result;
}