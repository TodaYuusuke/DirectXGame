#include "Interpolation.h"

// パイのため
#define _USE_MATH_DEFINES
#include <math.h>

using namespace LWP::Utility;
using namespace LWP::Math;

// 線形補間
float Interpolation::LerpF(const float& start, const float& end, const float& t) {
	return (1.0f - t) * start + t * end;
}
Vector3 Interpolation::Lerp(const Vector3& start, const Vector3& end, const float& t) {
	return (1.0f - t) * start + end * t;
}
// 球面線形補間
Vector3 Interpolation::Slerp(const Vector3& start, const Vector3& end, const float& t) {
	float dot = Vector3::Dot(start, end);
	float theta = static_cast<float>(std::acos((dot * M_PI) / 180.0f));
	float sinTheta = std::sin(theta);
	float weightStart = std::sin((1.0f - t) * theta) / sinTheta;
	float weightEnd = std::sin(t * theta) / sinTheta;

	return weightStart* start + weightEnd * end;
}
Quaternion Interpolation::SlerpQuaternion(const Quaternion& start, const Quaternion& end, const float& t) {
	Quaternion q1 = start;
	Quaternion q2 = end;
	
	// 内積を求める
	float dot = Quaternion::Dot(start, end);
	if (dot < 0) {
		q1 = q1 * -1.0f;
		dot = -dot;
	}
	// 内積が1を超えるときは線形補完にする
	if (dot >= 1.0f - DBL_EPSILON) {
		return (1.0f - t) * q1 + t * q2;
	}
	
	// なす角を求める
	float theta = std::acos(dot);

	// thetaを使って補間係数scale1とscale2を求める
	float scale1 = std::sin((1.0f - t) * theta) / std::sin(theta);
	float scale2 = std::sin(t * theta) / std::sin(theta);

	// それぞれの補間係数を利用して補間後のクォータニオンを求める
	return scale1 * q1 + scale2 * q2;
}

Vector3 Interpolation::Exponential(const Vector3& current, const Vector3& target, float damping) {
	float factor = 1.0f - std::exp(-damping);
	return current + (target - current) * factor;
}