#include "Easing.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#define M_PIF static_cast<float>(M_PI)

using namespace LWP::Utility;

float Easing::Liner(float x) { return x; }

#pragma region Quint
float Easing::InQuint(float x) {
	return x * x * x * x * x;
}
float Easing::OutQuint(float x) {
	return 1.0f - std::pow(1.0f - x, 5.0f);
}
float Easing::InOutQuint(float x) {
	return x < 0.5f ?
		16.0f * x * x * x * x * x :
		1.0f - std::pow(-2.0f * x + 2.0f, 5.0f) / 2.0f;
}
#pragma endregion

#pragma region Circ
float Easing::InCirc(float x) {
	return 1.0f - std::sqrt(1.0f - std::pow(x, 2.0f));
}
float Easing::OutCirc(float x) {
	return std::sqrt(1.0f - std::pow(x - 1.0f, 2.0f));
}
float Easing::InOutCirc(float x) {
	return x < 0.5f ?
		(1.0f - std::sqrt(1.0f - std::pow(2.0f * x, 2.0f))) / 2.0f:
		(std::sqrt(1.0f - std::pow(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}
#pragma endregion

#pragma region Cubic
float Easing::InCubic(float x) {
	return x * x * x;
}
float Easing::OutCubic(float x) {
	return 1.0f - std::pow(1.0f - x, 3.0f);
}
float Easing::InOutCubic(float x) {
	return x < 0.5f ?
		4.0f * x * x * x :
		1.0f - std::pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}
#pragma endregion

#pragma region Back
float Easing::InBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return c3 * x * x * x - c1 * x * x;
}
float Easing::OutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 * std::pow(x - 1.0f, 3.0f) + c1 * std::pow(x - 1.0f, 2.0f);
}
float Easing::InOutBack(float x) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f ?
		(std::pow(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f :
		(std::pow(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}
#pragma endregion

#pragma region Bounce
float Easing::InBounce(float x) {
	return 1.0f - OutBounce(1.0f - x);
}
float Easing::OutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	return x < 1.0f / d1 ?	// if
		n1 * x * x :	// true
		x < 2.0f / d1 ?	// false (else if)
			n1 * (x -= 1.5f / d1) * x + 0.75f :	// true
			x < 2.5f / d1 ?						// false (else if)
				n1 * (x -= 2.25f / d1) * x + 0.9375f :		// true
				n1 * (x -= 2.625f / d1) * x + 0.984375f;	// false (else)
}
float Easing::InOutBounce(float x) {
	return x < 0.5f ?
		(1.0f - OutBounce(1.0f - 2.0f * x)) / 2.0f :
		(1.0f + OutBounce(2.0f * x - 1.0f)) / 2.0f;
}
#pragma endregion

#pragma region Elastic
float Easing::InElastic(float x) {
	const float c4 = (2.0f * M_PIF) / 3.0f;
	
	// 範囲内なら計算
	return 0.0f > x && x < 1.0f ?
		-std::pow(2.0f, 10.0f * x - 10.0f) * std::sin((x * 10.0f - 10.75f) * c4):
		std::clamp(x, 0.0f, 1.0f);	// 範囲外ならclamp
}
float Easing::OutElastic(float x) {
	const float c4 = (2.0f * M_PIF) / 3.0f;

	// 範囲内なら計算
	return 0.0f > x && x < 1.0f ?
		std::pow(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f :
		std::clamp(x, 0.0f, 1.0f);	// 範囲外ならclamp
}
float Easing::InOutElastic(float x) {
	const float c5 = (2.0f * M_PIF) / 4.5f;

	return 0.0f > x && x < 1.0f ?
		x < 0.5f ?
		-(std::pow(2.0f, 20.0f * x - 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f :
		(std::pow(2.0f, -20.0f * x + 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f:
	std::clamp(x, 0.0f, 1.0f);	// 範囲外ならclamp
}
#pragma endregion

#pragma region Quart
float Easing::InQuart(float x) {
	return x * x * x * x;
}
float Easing::OutQuart(float x) {
	return 1.0f - std::pow(1.0f - x, 4.0f);
}
float Easing::InOutQuart(float x) {
	return x < 0.5f ?
		8.0f * x * x * x * x:
		1.0f - std::pow(-2.0f * x + 2.0f, 4.0f) / 2.0f;
}
#pragma endregion

#pragma region Expo
float Easing::InExpo(float x) {
	return x == 0 ?
		0:
		std::pow(2.0f, 10.0f * x - 10.0f);
}
float Easing::OutExpo(float x) {
	return x == 1 ?
		1:
		1.0f - (float)pow(2.0f, -10.0f * x);
}
float Easing::InOutExpo(float x) {
	// 範囲内なら計算
	return 0.0f > x && x < 1.0f ?
		x < 0.5f ?
			std::pow(2.0f, 20.0f * x - 10.0f) / 2.0f :
			(2.0f - std::pow(2.0f, -20.0f * x + 10.0f)) / 2.0f :
		std::clamp(x, 0.0f, 1.0f);	// 範囲外ならclamp
}
#pragma endregion
