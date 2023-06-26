#include "Vector3.h"

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
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}
/// クロス積を求める関数
Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}