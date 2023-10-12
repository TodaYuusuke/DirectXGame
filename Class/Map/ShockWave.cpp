#include "ShockWave.h"

using namespace LWP::Math;

void ShockWave::Initialize(Vector2 pos) {
	isActive = true;

	// 自身の円
	circle.center = pos;
	circle.radius = 0.0f;
	// 半径の広がる速度
	speed = 0.009f;
}
void ShockWave::Update() {
	if (!isActive) { return; }

	// 半径サイズ更新
	circle.radius += speed;
	if (circle.radius > kMaxRadius) {
		isActive = false;
	}
}
