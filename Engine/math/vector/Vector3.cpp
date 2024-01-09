#include "Vector3.h"
#include "../matrix/Matrix4x4.h"
#include <algorithm>

using namespace LWP::Math;

// *** オペーレーターオーバーロード *** //

// Vector3 Add(+) Vector3
Vector3 Vector3::operator+(const Vector3& other) const {
	return { x + other.x, y + other.y, z + other.z };
}
Vector3& Vector3::operator+=(const Vector3& other) {
	return *this = *this + other;
}
// Vector3 Subtract(-) Vector3
Vector3 Vector3::operator-(const Vector3& other) const {
	return { x - other.x, y - other.y, z - other.z };
}
Vector3& Vector3::operator-=(const Vector3& other) {
	return *this = *this - other;
}
// Vector3 Multiply(*) float
Vector3 Vector3::operator*(const float& other) const {
	return { x * other, y * other, z * other };
}
Vector3& Vector3::operator*=(const float& other) {
	return *this = *this * other;
}
// Vector3 Division(/) float
Vector3 Vector3::operator/(const float& other) const {
	return { x / other, y / other, z / other };
}
Vector3& Vector3::operator/=(const float& other) {
	return *this = *this / other;
}

Vector3 Vector3::operator*(const Matrix4x4& other) const {
	Vector3 result{};
	result.x = x * other.m[0][0] + y * other.m[1][0] + z * other.m[2][0] + other.m[3][0];
	result.y = x * other.m[0][1] + y * other.m[1][1] + z * other.m[2][1] + other.m[3][1];
	result.z = x * other.m[0][2] + y * other.m[1][2] + z * other.m[2][2] + other.m[3][2];
	return result;
}

/// 3次元ベクトルの長さを求める
float Vector3::Length() {
	return sqrtf(x * x + y * y + z * z);
}
/// 3次元ベクトルの正規化
Vector3 Vector3::Normalize() {
	Vector3 norm = { x,y,z };
	float length = Length();
	if (length != 0.0f) {
		norm.x /= length;
		norm.y /= length;
		norm.z /= length;
	}
	return norm;
}

// *** 静的なメンバ関数 *** //

/// 3次元ベクトルの内積を求める
float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
/// クロス積を求める関数
Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

// Catmull-Romスプライン曲線補間
Vector3 Vector3::CatmullRom(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t) {
	return (((p0 * -1) + (p1 * 3) + (p2 * -3) + (p3)) * powf(t, 3.0f) +
		((p0 * 2) + (p1 * -5) + (p2 * 4) + (p3 * -1)) * powf(t, 2.0f) + ((p0 * -1) + (p2)) * t +
		(p1 * 2)) *
		(1.0f / 2.0f);
}
// Catmull-Romスプライン曲線補間の座標を求める
Vector3 Vector3::GetCatmullRomPosition(std::vector<Vector3> controlPoints, float t) {
	// 4頂点未満の場合計算しない
	if (controlPoints.size() < 4) {
		assert(false);
	}

	// サイズに応じて計算する4点とtの値を求める
	float calcT = t * (controlPoints.size() - 1);
	// 整数部を取得
	int roop = static_cast<int>(calcT);
	if (t == 1.0f) {
		roop -= 1;
	}
	// 小数部を取得
	if (t == 1.0f) {
		calcT = 1.0f;
	}
	else {
		calcT -= static_cast<float>(roop);
	}

	roop = std::clamp(roop, 0, static_cast<int>(controlPoints.size() - 1));
	calcT = std::clamp(calcT, 0.0f, 1.0f);

	// 計算する4点を求める
	Vector3 pos0{}, pos1{}, pos2{}, pos3{};
	// 最初のイテレータ
	std::vector<Vector3>::iterator itr = controlPoints.begin();
	for (int i = 0; i <= roop; i++) {

		// pos0は始点の場合 -> pos1と同じ
		if (itr == controlPoints.begin()) {
			pos0 = *itr; // 同じ
			pos1 = *itr;
		}
		else {
			pos0 = *--itr; // ここでマイナスしてるので
			pos1 = *++itr; // プラスする
		}
		pos2 = *++itr;
		// pos3は終点の場合 -> pos2と同じ
		if (itr == --controlPoints.end()) {
			pos3 = *itr;
		}
		else {
			pos3 = *++itr;
			--itr; // 戻す
		}
	}

	// 頂点座標を計算
	return CatmullRom(pos0, pos1, pos2, pos3, calcT);
}
