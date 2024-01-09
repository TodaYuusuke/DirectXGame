#include "EffectManager.h"

EffectManager* EffectManager::GetInstance() {
	static EffectManager instance;
	return &instance;
}

void EffectManager::Initialize() {
	for (int i = 0; i < kEffectAmount; i++) {
		effect[i].Initialize();
	}
}
void EffectManager::Update() {
	for (int i = 0; i < kEffectAmount; i++) {
		effect[i].Update();
	}
}

void EffectManager::Start(LWP::Math::Vector3 position) {
	for (int i = 0; i < kEffectAmount; i++) {
		if (effect[i].Start(position)) {
			break;
		}
	}
}