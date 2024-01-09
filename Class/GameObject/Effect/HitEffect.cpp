#include "HitEffect.h"

void HitEffect::Initialize() {
	for (int i = 0; i < kSphereAmount; i++) {
		spheres[i] = LWP::Primitive::CreateInstance<LWP::Primitive::Sphere>();
		spheres[i]->Radius(0.3f);
		spheres[i]->Subdivision(6);
		spheres[i]->isActive = false;
		isStarted = false;
	}
}
void HitEffect::Update() {
	if (!isStarted) { return; }

	elapsedFrame++;
	if (elapsedFrame > kEffectFrame) {
		isStarted = false;
		for (int i = 0; i < kSphereAmount; i++) {
			spheres[i]->isActive = false;
		}
		return;
	}

	for (int i = 0; i < kSphereAmount; i++) {
		spheres[i]->transform.translation += velocity[i];
	}
}

bool HitEffect::Start(LWP::Math::Vector3 position) {
	if (isStarted) { return false; }

	isStarted = true;
	elapsedFrame = 0;
	
	for (int i = 0; i < kSphereAmount; i++) {
		spheres[i]->isActive = true;
		spheres[i]->transform.translation = position;
		velocity[i] = {
			float(LWP::Utility::GenerateRandamNum<int>(-50,50)) / 100.0f,
			float(LWP::Utility::GenerateRandamNum<int>(-50,50)) / 100.0f,
			float(LWP::Utility::GenerateRandamNum<int>(-50,50)) / 100.0f
		};
	}

	return true;
}