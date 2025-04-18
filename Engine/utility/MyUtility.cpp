#include "MyUtility.h"

#include <numbers>

using namespace LWP::Math;

float LWP::Utility::RadianToDegree(float radian) {
	return radian * (180.0f / static_cast<float>(std::numbers::pi));
}

float LWP::Utility::DegreeToRadian(float degree) {
	return degree * (static_cast<float>(std::numbers::pi) / 180.0f);
}


float LWP::Utility::GetRadian(const Vector3& vec1, const Vector3& vec2) {
	Vector3 vec1Norm = vec1.Normalize();
	Vector3 vec2Norm = vec2.Normalize();

	float dot = LWP::Math::Vector3::Dot(vec1Norm, vec2Norm);
	dot = std::fmax(-1.0f, std::fmin(1.0f, dot)); // 数値誤差対策

	float angle = std::acos(dot); // 0〜π

	Vector3 cross = Vector3::Cross(vec1Norm, vec2Norm);

	if (Vector3::Dot(Vector3::UnitY(), cross) < 0) {
		angle = 2.0f * static_cast<float>(std::numbers::pi) - angle;
	}

	return angle;
}