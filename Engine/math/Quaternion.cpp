#include "Quaternion.h"
#include "vector/Vector3.h"

using namespace LWP::Math;



Quaternion::Quaternion(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Quaternion::Init() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

Quaternion Quaternion::Normalize() const {
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
Quaternion Quaternion::Conjugate() const {
	// 結果を返す
	return { x * -1.0f,y * -1.0f,z * -1.0f,w };
}

float Quaternion::Length() const {
	return sqrtf(powf(w, 2) + powf(x, 2) + powf(y, 2) + powf(z, 2));
}
Quaternion Quaternion::Inverse() const {
	Quaternion result{};
	// 正規化するベクトルの長さを求める
	float length = Length();
	length = std::powf(length, 2);
	// 共役クォータニオンを求める
	Quaternion conjugate = Conjugate();

	// 計算処理
	if (length != 0.0f) {
		result.x = conjugate.x / length;
		result.y = conjugate.y / length;
		result.z = conjugate.z / length;
		result.w = conjugate.w / length;
	}

	// 結果を返す
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

float Quaternion::Dot(const Quaternion& v1, const Quaternion& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}
Quaternion Quaternion::ConvertDirectionVector(const Vector3& dir) {
	// 向きベクトルを正規化する
	Vector3 normalizedDir = dir.Normalize();

	// ベクトルと(0, 0, 1)のなす角の半分を計算する
	float angle = std::acos(Vector3::Dot(Vector3(0.0f, 0.0f, 1.0f), normalizedDir)) / 2.0f;
	// 回転軸を計算する
	Vector3 axis = Vector3::Cross(Vector3(0.0f, 0.0f, 1.0f), normalizedDir);

	// 回転軸と角度からクォータニオンを生成する
	Quaternion result;

	float sin = std::sin(angle);

	result.x = axis.x * sin;
	result.y = axis.y * sin;
	result.z = axis.z * sin;
	result.w = std::cos(angle);

	return result.Normalize();
}
Quaternion Quaternion::ConvertEuler(const Vector3& eulerAngle) {
	float cx = std::cos(0.5f * eulerAngle.x);
	float sx = std::sin(0.5f * eulerAngle.x);
	float cy = std::cos(0.5f * eulerAngle.y);
	float sy = std::sin(0.5f * eulerAngle.y);
	float cz = std::cos(0.5f * eulerAngle.z);
	float sz = std::sin(0.5f * eulerAngle.z);
	return Quaternion(
		cx * sy * sz + sx * cy * cz,
		-sx * cy * sz + cx * sy * cz,
		cx * cy * sz + sx * sy * cz,
		-sx * sy * sz + cx * cy * cz
	);
}

Quaternion Quaternion::operator+ (const Quaternion other) const {
	Quaternion result;
	result.x = this->x + other.x;
	result.y = this->y + other.y;
	result.z = this->z + other.z;
	result.w = this->w + other.w;
	return result;
}
Quaternion Quaternion::operator+= (const Quaternion other) { return *this = *this + other; }

Quaternion Quaternion::operator*(const Quaternion& other) const {
	Quaternion result;
	Vector3 cross = Vector3::Cross({ x,y,z }, { other.x,other.y,other.z });
	float dot = Vector3::Dot({ x,y,z }, { other.x,other.y,other.z });
	result = cross + (other.w * Vector3{ x,y,z }) + (w * Vector3{ other.x,other.y,other.z });
	result.w = w * other.w - dot;
	return result;
}
Quaternion& Quaternion::operator*=(const Quaternion& other) { return *this = *this * other; }

Quaternion Quaternion::operator*(const Vector3& other) const {
	Quaternion vec;
	vec = other;
	return Quaternion(x,y,z,w) * vec * Inverse();
}
Quaternion& Quaternion::operator*=(const Vector3& other) { return *this = *this * other; }

Quaternion Quaternion::operator* (const float& other) const {
	Quaternion result;
	result.x = this->x * other;
	result.y = this->y * other;
	result.z = this->z * other;
	result.w = this->w * other;
	return result;
}
Quaternion& Quaternion::operator*=(const float& other) { return *this = *this * other; }

Quaternion Quaternion::operator/ (const Quaternion& other) const {
	Quaternion result;
	result.x = this->x / other.x;
	result.y = this->y / other.y;
	result.z = this->z / other.z;
	result.w = this->w / other.w;
	return result;
}
Quaternion Quaternion::operator/= (const Quaternion& other) { return *this = *this / other; }

Quaternion Quaternion::operator=(const Vector3& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	return *this;
}
bool Quaternion::operator==(const Quaternion& other) const {
	return {x == other.x &&
		y == other.y &&
		z == other.z &&
		w == other.w };
}