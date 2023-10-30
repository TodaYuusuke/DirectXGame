#include "MapChip.h"

using namespace LWP::Math;

void MapChip::Initialize() {
	model = LWP::Resource::LoadModel("groundCube/groundCube.obj");
	model->material.enableLighting = true;
}
void MapChip::Update() {
	// パワーは徐々に減衰する
	wavePower = wavePower * 0.9f;

	// 自身が持つパワーの大きさに応じてscaleを大きくする
	model->transform.scale.y = wavePower * 0.1f;
	// スケールが0にならないようにする
	if (model->transform.scale.y < 0.01f) {
		model->transform.scale.y = 0.01f;
	}
}

bool MapChip::IsInCircle(Circle circle) {
	// Y軸は無視して考える
	Vector2 center = { model->transform.translation.x,model->transform.translation.z };
	// 点と円の距離を計算
	float distance = std::sqrt((center.x - circle.center.x) * (center.x - circle.center.x) + (center.y - circle.center.y) * (center.y - circle.center.y));

	// 点と円の距離が半径未満ならば、点は円内にある
	return distance < circle.radius;
}
