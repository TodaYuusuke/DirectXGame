#include "Quaternion.h"
#include "vector/Vector3.h"

using namespace LWP::Math;

Quaternion Quaternion::operator*(const Quaternion& other) const {
	Quaternion result;
	Vector3 cross = Vector3::Cross({x,y,z}, { other.x,other.y,other.z });
	float dot = Vector3::Dot({ x,y,z }, { other.x,other.y,other.z });
	result = cross + (other.w * Vector3{ x,y,z }) + (w * Vector3{ other.x,other.y,other.z });
	result.w = w * other.w - dot;
	return result;
}
Quaternion& Quaternion::operator*=(const Quaternion& other) {
	return *this = *this * other;
}
Quaternion Quaternion::operator=(const Vector3& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	return *this;
}

void Quaternion::Init() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}
Quaternion Quaternion::Normalize() {
	Quaternion result{};
	float norm = std::sqrt(x * x + y * y + z * z + w * w);
	if (norm != 0.0f) {
		result.x = x / norm;
		result.y = y / norm;
		result.z = z / norm;
		result.w = w / norm;
	}
	return result;
}

Quaternion Quaternion::CreateRotationX(const Vector3& start, const Vector3& end) {
	return CreateRotation(start, end, { 0.0f,1.0f,1.0f });
}
Quaternion Quaternion::CreateRotationY(const Vector3& start, const Vector3& end) {
	return CreateRotation(start, end, { 1.0f,0.0f,1.0f });
}
Quaternion Quaternion::CreateRotationZ(const Vector3& start, const Vector3& end) {
	return CreateRotation(start, end, { 1.0f,1.0f,0.0f });
}
Quaternion Quaternion::CreateRotation(const Vector3& start, const Vector3& end, const Vector3& axis) {
	// 向き
	Vector3 dir = (end - start).Normalize();

	Vector3 cross = (Vector3::Cross(axis, dir)).Normalize();
	float dot = Vector3::Dot(axis, dir);

	// 結果
	Quaternion result;

	float halfAngle = std::acos(dot) / 2.0f;
	float sin = std::sin(halfAngle);

	result.x = axis.x * sin;
	result.y = axis.y * sin;
	result.z = axis.z * sin;
	result.w = std::cos(halfAngle);

	return result.Normalize();
}
