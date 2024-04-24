#pragma once

namespace LWP::Utility::Easing {
	enum class Type : int {
		Liner,
		InQuint,
		OutQuint,
		InOutQuint,
		InCirc,
		OutCirc,
		InOutCirc,
		InCubic,
		OutCubic,
		InOutCubic,
		InBack,
		OutBack,
		InOutBack,
		InBounce,
		OutBounce,
		InOutBounce,
		InElastic,
		OutElastic,
		InOutElastic,
		InQuart,
		OutQuart,
		InOutQuart,
		InExpo,
		OutExpo,
		InOutExpo,
		EasingCount	// 個数カウント用
	};

	// 直線補間
	float Liner(float x);

#pragma region Quint
	float InQuint(float x);
	float OutQuint(float x);
	float InOutQuint(float x);
#pragma endregion

#pragma region Circ
	float InCirc(float x);
	float OutCirc(float x);
	float InOutCirc(float x);
#pragma endregion

#pragma region Cubic
	float InCubic(float x);
	float OutCubic(float x);
	float InOutCubic(float x);
#pragma endregion

#pragma region Back
	float InBack(float x);
	float OutBack(float x);
	float InOutBack(float x);
#pragma endregion

#pragma region Bounce
	float InBounce(float x);
	float OutBounce(float x);
	float InOutBounce(float x);
#pragma endregion

#pragma region Elastic
	float InElastic(float x);
	float OutElastic(float x);
	float InOutElastic(float x);
#pragma endregion

#pragma region Quart
	float InQuart(float x);
	float OutQuart(float x);
	float InOutQuart(float x);
#pragma endregion

#pragma region Expo
	float InExpo(float x);
	float OutExpo(float x);
	float InOutExpo(float x);
#pragma endregion
};