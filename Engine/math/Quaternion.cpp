#include "Quaternion.h"
#include "vector/Vector3.h"

#include <numbers>
#include <cmath>


using namespace LWP::Math;


Quaternion::Quaternion(const Vector3& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	w = 0.0f;
}
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
	//Quaternion result{};
	//float norm = std::sqrt(x * x + y * y + z * z + w * w);
	//if (norm != 0.0f) {
	//	result.x = x / norm;
	//	result.y = y / norm;
	//	result.z = z / norm;
	//	result.w = w / norm;
	//}
	//return result;

	// 正規化するベクトルの長さを求める
	float length = Length();
	// 結果格納用
	Quaternion result;

	// 計算処理
	if (length != 0.0f) {
		result.x = x / length;
		result.y = y / length;
		result.z = z / length;
		result.w = w / length;
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
Quaternion Quaternion::Conjugate() const {
	// 結果を返す
	return { x * -1.0f,y * -1.0f,z * -1.0f,w };

	//// 結果格納用
	//Quaternion result = *this;

	//// 虚部を反転
	//result.x *= -1.0f;
	//result.y *= -1.0f;
	//result.z *= -1.0f;

	//// 結果を返す
	//return result;
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

Quaternion Quaternion::CreateFromAxisAngle(const Vector3& axis, float radian) {
	// 回転軸の正規化
	Vector3 normalizedAxis = axis.Normalize();
	// 回転角度の半分
	float halfAngle = radian * 0.5f;

	// 回転クォータニオンの成分
	float s = std::sinf(halfAngle);
	float c = std::cosf(halfAngle);

	// 回転クォータニオンを作成
	Quaternion result;
	result.x = normalizedAxis.x * s;
	result.y = normalizedAxis.y * s;
	result.z = normalizedAxis.z * s;
	result.w = c;

	return result.Normalize();
}
Quaternion Quaternion::DirectionToDirection(const Vector3& from, const Vector3& to) {
	// 回転軸をクロス積から求める
	Vector3 axis = Vector3::Cross(from, to);
	// 内積
	float dot = Vector3::Dot(from, to);
	// 完全に平行な場合、単位クォータニオンを返す
    if (dot > 0.9999f) {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

	// 完全に逆方向の場合、任意の直交するベクトルを回転軸として180度回転クォータニオンを返す
	//if (dot < -0.9999f) {
	//	Vector3 orthogonalAxis = { 1.0f, 0.0f, 0.0f };
	//	if (std::abs(from.x) > 0.9f) {
	//		orthogonalAxis = { 0.0f, 1.0f, 0.0f };
	//	}
	//	Vector3 axis = Vector3::Cross(from, orthogonalAxis);
	//	axis.Normalize();
	//	return Quaternion::CreateFromAxisAngle(axis, std::numbers::pi_v<float>); // 180度回転
	//}

	// θを求める
	float theta = std::acos(Vector3::Dot(from, to) / (from.Length() * to.Length()));

	return CreateFromAxisAngle(axis, theta);
}


float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2) {
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}
Quaternion Quaternion::ConvertDirection(const Vector3& dir) {
	// 方向ベクトルを正規化する
	Vector3 forward = dir.Normalize();

	// ベクトルが上向きであるかどうかをチェックし、上向きでない場合は回転を調整する
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	if (std::abs(Vector3::Dot(forward, up)) > 0.999f) {
		// ベクトルが上向きである場合、up ベクトルを別の値に変更する
		up = { 0.0f, 0.0f, -1.0f };
	}

	// Z軸方向の回転を求める
	Quaternion rotation = Quaternion::ConvertFromTo(Vector3::UnitZ(), forward);

	// 上向きベクトルと forward ベクトルの間の回転を求める
	Vector3 right = Vector3::Cross(up, forward).Normalize();
	Vector3 correctedUp = Vector3::Cross(forward, right).Normalize();
	Quaternion rotation2 = Quaternion::ConvertFromTo(Vector3::UnitY(), correctedUp);

	// 二つの回転を合成する
	return rotation2 * rotation;
}

Quaternion Quaternion::ConvertFromTo(const Vector3& from, const Vector3& to) {
	// 回転軸の計算
	Vector3 axis = Vector3::Cross(from, to).Normalize();

	// 回転角の計算
	float dot = Vector3::Dot(from, to);
	float angle = std::acos(dot);

	// クォータニオンの生成
	return CreateFromAxisAngle(axis, angle);
}
Quaternion Quaternion::ConvertRotateMatrix(const Matrix4x4& r) {
	float px = r.m[0][0] - r.m[1][1] - r.m[2][2] + 1;
	float py = -r.m[0][0] + r.m[1][1] - r.m[2][2] + 1;
	float pz = -r.m[0][0] - r.m[1][1] + r.m[2][2] + 1;
	float pw = r.m[0][0] + r.m[1][1] + r.m[2][2] + 1;

	float selected = 0;
	float max = px;
	if (max < py) {
		selected = 1;
		max = py;
	}
	if (max < pz) {
		selected = 2;
		max = pz;
	}
	if (max < pw) {
		selected = 3;
		max = pw;
	}

	if (selected == 0) {
		float x = std::sqrt(px) * 0.5f;
		float d = 1 / (4 * x);
		return Quaternion(
			x,
			(r.m[1][0] + r.m[0][1]) * d,
			(r.m[0][2] + r.m[2][0]) * d,
			(r.m[2][1] - r.m[1][2]) * d
		);
	}
	else if (selected == 1) {
		float y = std::sqrt(py) * 0.5f;
		float d = 1 / (4 * y);
		return Quaternion(
			(r.m[1][0] + r.m[0][1]) * d,
			y,
			(r.m[2][1] + r.m[1][2]) * d,
			(r.m[0][2] - r.m[2][0]) * d
		);
	}
	else if (selected == 2) {
		float z = std::sqrt(pz) * 0.5f;
		float d = 1 / (4 * z);
		return Quaternion(
			(r.m[0][2] + r.m[2][0]) * d,
			(r.m[2][1] + r.m[1][2]) * d,
			z,
			(r.m[1][0] - r.m[0][1]) * d
		);
	}
	else /* if (selected == 3)*/ {
		float w = std::sqrt(pw) * 0.5f;
		float d = 1 / (4 * w);
		return Quaternion(
			(r.m[2][1] - r.m[1][2]) * d,
			(r.m[0][2] - r.m[2][0]) * d,
			(r.m[1][0] - r.m[0][1]) * d,
			w
		);
	}
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
Vector3 Quaternion::ConvertQuaternion(const Quaternion& q) {
	float sy = 2.0f * q.x * q.z + 2 * q.y * q.w;
	float unlocked = std::abs(sy) < 0.99999f;
	return Vector3(
		unlocked ? std::atan2(-(2 * q.y * q.z - 2 * q.x * q.w), 2 * q.w * q.w + 2 * q.z * q.z - 1)
		: std::atan2(2 * q.y * q.z + 2 * q.x * q.w, 2 * q.w * q.w + 2 * q.y * q.y - 1),
		std::asin(sy),
		unlocked ? std::atan2(-(2 * q.x * q.y - 2 * q.z * q.w), 2 * q.w * q.w + 2 * q.x * q.x - 1) : 0
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

Quaternion Quaternion::operator*(const float& other) const {
	Quaternion result;
	result.x = this->x * other;
	result.y = this->y * other;
	result.z = this->z * other;
	result.w = this->w * other;
	return result;
}
Quaternion& Quaternion::operator*=(const float& other) { return *this = *this * other; }

Quaternion Quaternion::operator/(const Quaternion& other) const {
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