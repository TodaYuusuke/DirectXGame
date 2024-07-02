#include "Interpolation.h"

// パイのため
#define _USE_MATH_DEFINES
#include <math.h>

using namespace LWP::Utility;
using namespace LWP::Math;

// 線形補間
//float Interpolation::Lerp(const float& start, const float& end, const float& t) {
//	return (1.0f - t) * start + t * end;
//}
Vector3 Interpolation::Lerp(const Vector3& start, const Vector3& end, const float& t) {
	//return (1.0f - t) * start + t * end;
	return (start * (1.0f - t) + end * t);
}
// 球面線形補間
//float Interpolation::Slerp(const float& start, const float& end, const float& t) {
//
//}
Vector3 Interpolation::Slerp(const Vector3& start, const Vector3& end, const float& t) {
	float dot = Vector3::Dot(start, end);
	float theta = static_cast<float>(std::acos((dot * M_PI) / 180.0f));
	float sinTheta = std::sin(theta);
	float weightStart = std::sin((1.0f - t) * theta) / sinTheta;
	float weightEnd = std::sin(t * theta) / sinTheta;

	return weightStart* start + weightEnd * end;
}
Quaternion Interpolation::SlerpQuaternion(const Quaternion& start, const Quaternion& end, const float& t) {
	// 結果格納用
	Quaternion result{};

	// クォータニオンの内積で求める
	float dot = Quaternion::Dot(start, end);

	Quaternion s = start;
	Quaternion e = end;
	if (dot < 0) {
		// 逆の回転を使う
		s = s.Inverse();
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
		result = (start * (std::sin(theta * (1.0f - t)) * sinTheta)) + (e.Inverse() * (std::sin(theta * t) * sinTheta));
	}
	else {
		result = (start * (std::sin(theta * (1.0f - t)) * sinTheta)) + (end * (std::sin(theta * t) * sinTheta));
	}
	// 結果を返す
	return result;
}