#include "Vector4.h"

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