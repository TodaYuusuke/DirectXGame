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

	static LWP::Object::WorldTransform EaseOutQuint(
		LWP::Object::WorldTransform s,
		LWP::Object::WorldTransform e,
		float t
	);
	
private:

	
};

