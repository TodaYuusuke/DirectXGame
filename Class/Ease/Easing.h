#pragma once
#include "../Engine/object/WorldTransform.h"

class Easing
{
public:// ** メンバ関数 ** //

	static LWP::Object::WorldTransform EaseOutBounce(
		LWP::Object::WorldTransform s,
		LWP::Object::WorldTransform e,
		float t
	);
	
	static LWP::Object::WorldTransform ScaleEaseOutQuint(
		LWP::Object::WorldTransform s,
		LWP::Object::WorldTransform e,
		float t
	);

	static LWP::Object::WorldTransform EaseOutQuint(
		LWP::Object::WorldTransform s,
		LWP::Object::WorldTransform e,
		float t
	);
	
	static int32_t EaseinQuad(
		int32_t s,
		int32_t e,
		float t
	);

private:

	
};

