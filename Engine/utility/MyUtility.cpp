#include "MyUtility.h"

#define _USE_MATH_DEFINES
#include <math.h>

float LWP::Utility::RadianToDegree(float radian) {
	return radian * (180.0f / static_cast<float>(M_PI));
}

float LWP::Utility::DegreeToRadian(float degree) {
	return degree * (static_cast<float>(M_PI) / 180.0f);
}