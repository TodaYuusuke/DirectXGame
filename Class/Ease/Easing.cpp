#include "Easing.h"


LWP::Object::WorldTransform Easing::EaseOutBounce(
	LWP::Object::WorldTransform s,
	LWP::Object::WorldTransform e,
	float t
) {
	LWP::Object::WorldTransform result;
	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float EasedT = t;
	if (t < 1.0f / d1) 
	{
		EasedT = n1 * EasedT * EasedT;
	}
	else if (t < 2.0f / d1) 
	{
		EasedT = n1 * (EasedT -= 1.5f / d1) * EasedT + 0.75f;
	}
	else if (t < 2.5f / d1) 
	{
		EasedT = n1 * (EasedT -= 2.25f / d1) * EasedT + 0.9375f;
	}
	else
	{
		EasedT = n1 * (EasedT -= 2.625f / d1) * EasedT + 0.984375f;
	}

	result.translation.x = (1.0f - EasedT) * s.translation.x + EasedT * e.translation.x;
	result.translation.y = (1.0f - EasedT) * s.translation.y + EasedT * e.translation.y;
	return result;
}