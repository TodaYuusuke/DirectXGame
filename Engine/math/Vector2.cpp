#include "Vector2.h"
#include "Matrix4x4.h"

using namespace LWP::Math;

// *** オペーレーターオーバーロード *** //

// Vector2 Add(+) Vector2
Vector2 Vector2::operator+(const Vector2& other) const {
	return { x + other.x, y + other.y };
}
Vector2& Vector2::operator+=(const Vector2& other) {
	return *this = *this + other;
}
// Vector2 Subtract(-) Vector2
Vector2 Vector2::operator-(const Vector2& other) const {
	return { x - other.x, y - other.y };
}
Vector2& Vector2::operator-=(const Vector2& other) {
	return *this = *this - other;
}
// Vector2 Multiply(*) float
Vector2 Vector2::operator*(const float& other) const {
	return { x * other, y * other };
}
Vector2& Vector2::operator*=(const float& other) {
	return *this = *this * other;
}
Vector2 Vector2::operator*(const Matrix4x4& other) const {
	Vector2 result{};
	result.x = x * other.m[0][0] + y * other.m[1][0] + other.m[2][0] + other.m[3][0];
	result.y = x * other.m[0][1] + y * other.m[1][1] + other.m[2][1] + other.m[3][1];
	return result;
}