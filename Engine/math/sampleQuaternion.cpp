#include "Quaternion.h"

using namespace LWP::Math;

Quaternion::Quaternion()
{
	// 単位クォータニオンの生成
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(const Vector3& v)
{
	// 乗法単位元
	x = v.x;
	y = v.y;
	z = v.z;
	w = 0.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w) noexcept
{
	// 乗法単位元
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline Vector3& Quaternion::vec()
{
	return *reinterpret_cast<Vector3*>(&x);
}

inline const Vector3& Quaternion::vec() const
{
	return *reinterpret_cast<const Vector3*>(&x);
}

Quaternion Quaternion::operator+(const Quaternion q) const
{
	Quaternion result = *this;
	result.vec() = { this->x + q.vec().x, this->y + q.vec().y, this->z + q.vec().z };
	result.w = this->w + q.w;
	return result;
}

Quaternion Quaternion::operator*(const Quaternion q) const
{
	Quaternion result;
	result.vec() = Vector3::Cross(this->vec(), q.vec()) + (this->vec() * q.w) + (q.vec() * this->w);
	result.w = this->w * q.w - Vector3::Dot(this->vec(), q.vec());
	return result;
}

Quaternion Quaternion::operator*(const float f) const
{
	Quaternion result = *this;

	result.vec() = result.vec() * f;
	result.w = result.w * f;

	return result;
}

Quaternion Quaternion::operator/(const Quaternion q) const
{
	Quaternion result;
	result.x = this->x / q.x;
	result.y = this->y / q.y;
	result.z = this->z / q.z;
	result.w = this->w / q.w;
	return result;
}

bool Quaternion::operator==(const Quaternion& other) const {
	return { x == other.x &&
		y == other.y &&
		z == other.z &&
		w == other.w };
}

Quaternion Quaternion::Conjugate(const Quaternion& q)
{
	// 結果格納用
	Quaternion result;
	// 引数を取得
	result = q;

	// 虚部を反転
	result.x *= -1.0f;
	result.y *= -1.0f;
	result.z *= -1.0f;

	// 結果を返す
	return result;
}

float Quaternion::Length(const Quaternion& q)
{
	// 結果格納用
	float result;

	// 計算処理
	result = sqrtf(powf(q.w, 2) + powf(q.x, 2) + powf(q.y, 2) + powf(q.z, 2));

	// 結果を返す
	return result;
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	// 正規化するベクトルの長さを求める
	float length = Length(q);
	// 結果格納用
	Quaternion result;

	// 計算処理
	if (length != 0.0f) {
		result.x = q.x / length;
		result.y = q.y / length;
		result.z = q.z / length;
		result.w = q.w / length;
	}
	else {
		result.x = 0.0f;
		result.y = 0.0f;
		result.z = 0.0f;
		result.w = 0.0f;
	}

	// 結果を返す
	return result;
}

float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
	// 結果格納用
	float result = 0.0f;

	// 計算
	result = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);

	// 結果を返す
	return result;
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
	// 結果格納用
	Quaternion result = Quaternion();

	// 正規化するベクトルの長さを求める
	float length = Length(q);
	length = std::powf(length, 2);
	// 共役クォータニオンを求める
	Quaternion conjugate = Conjugate(q);

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


Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& v, float angle)
{
	// 結果格納用
	Quaternion result = Quaternion();

	// 計算処理
	result.w = cos((angle / 2.0f));
	result.x = v.x * sin((angle / 2.0f));
	result.y = v.y * sin((angle / 2.0f));
	result.z = v.z * sin((angle / 2.0f));

	// 結果を返す
	return result;
}

Vector3 Quaternion::RoatateVector(const Vector3& v, const Quaternion& q)
{
	// 引数のベクトルを使用してクォータニオンを生成する
	Quaternion vq = Quaternion();
	vq.x = v.x;
	vq.y = v.y;
	vq.z = v.z;
	vq.w = 0.0f;

	// 生成したクォータニオンを用いてクォータニオンを回転させる
	Quaternion result = q * vq * Conjugate(q);
	// 計算結果を返す
	return result.vec();
}

Matrix4x4 Quaternion::QuaternionToMatrix(const Quaternion& q)
{
	// 結果格納用
	Matrix4x4 result = Matrix4x4();

	// 計算処理
	result.m[0][0] = powf(q.w, 2) + powf(q.x, 2) - powf(q.y, 2) - powf(q.z, 2);
	result.m[0][1] = 2.0f * (q.x * q.y + q.w * q.z);
	result.m[0][2] = 2.0f * (q.x * q.z - q.w * q.y);

	result.m[1][0] = 2.0f * (q.x * q.y - q.w * q.z);
	result.m[1][1] = powf(q.w, 2) - powf(q.x, 2) + powf(q.y, 2) - powf(q.z, 2);
	result.m[1][2] = 2.0f * (q.y * q.z + q.w * q.x);

	result.m[2][0] = 2.0f * (q.x * q.z + q.w * q.y);
	result.m[2][1] = 2.0f * (q.y * q.z - q.w * q.x);
	result.m[2][2] = powf(q.w, 2) - powf(q.x, 2) - powf(q.y, 2) + powf(q.z, 2);

	// 計算結果を返す
	return result;
}

Quaternion Quaternion::Slerp(float t, const Quaternion& start, const Quaternion& end)
{
	// 結果格納用
	Quaternion result = Quaternion();

	// クォータニオンの内積で求める
	float dot = Dot(start, end);

	Quaternion s = start;
	Quaternion e = end;
	if (dot < 0) {
		// 逆の回転を使う
		s = Inverse(s);
		// 内積も逆
		dot = -dot;
	}

	// クォータニオンが成す角を求める
	float theta = std::acos(dot);
	// sin角も求める
	float sinTheta = 1.0f / std::sin(theta);

	// 0.0により近いか
	if (dot <= -1.0f || 1.0f <= dot || sinTheta == 0.0f) {
		result = s * (1.0f - t) + (e * t);
	}
	// 近いほうで補完する
	else if (dot < 0.0f) {
		result = (start * (std::sin(theta * (1.0f - t)) * sinTheta)) + (Inverse(end) * (std::sin(theta * t) * sinTheta));
	}
	else {
		result = (start * (std::sin(theta * (1.0f - t)) * sinTheta)) + (end * (std::sin(theta * t) * sinTheta));
	}
	// 結果を返す
	return result;
}
