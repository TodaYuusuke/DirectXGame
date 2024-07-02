#include "Vector4.h"
#include "../matrix/Matrix4x4.h"

using namespace LWP::Math;

// *** オペーレーターオーバーロード *** //

// Vector4 Add(+) Vector4
Vector4 Vector4::operator+(const Vector4& other) const {
	return { x + other.x, y + other.y, z + other.z, w + other.w };
}
Vector4& Vector4::operator+=(const Vector4& other) {
	return *this = *this + other;
}
// Vector4 Subtract(-) Vector4
Vector4 Vector4::operator-(const Vector4& other) const {
	return { x - other.x, y - other.y, z - other.z, w - other.w };
}
Vector4& Vector4::operator-=(const Vector4& other) {
	return *this = *this - other;
}
// Vector4 Multiply(*) float
Vector4 Vector4::operator*(const float& other) const {
	return { x * other, y * other, z * other, w * other };
}
Vector4& Vector4::operator*=(const float& other) {
	return *this = *this * other;
}

Vector4 Vector4::operator*(const Matrix4x4& other) const {
	Vector4 result;
	result.x = other.m[0][0] * x + other.m[0][1] * y + other.m[0][2] * z + other.m[0][3] * w;
	result.y = other.m[1][0] * x + other.m[1][1] * y + other.m[1][2] * z + other.m[1][3] * w;
	result.z = other.m[2][0] * x + other.m[2][1] * y + other.m[2][2] * z + other.m[2][3] * w;
	result.w = other.m[3][0] * x + other.m[3][1] * y + other.m[3][2] * z + other.m[3][3] * w;
	return result;
}
Vector4& Vector4::operator*=(const Matrix4x4& other) {
	return *this = *this * other;
}