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
	return (1.0f - t) * start + t * end;
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