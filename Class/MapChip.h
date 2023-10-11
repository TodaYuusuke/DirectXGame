#pragma once
#include "../Adapter/Adapter.h"

// 円を示す構造体
struct Circle {
	// 中心点
	LWP::Math::Vector2 center;
	// 半径
	float radius;
};

class MapChip {
public:
	void Initialize();
	void Update();

	// 円の中かを判別する
	bool IsInCircle(Circle circle);

	// モデル
	LWP::Primitive::Mesh* model = nullptr;

	// マップが場外か判定
	bool isOut = true;

	// 自身の持つ波のパワー
	float wavePower = 0.0f;
	// 波のベクトル
	LWP::Math::Vector2 waveVec = { 0.0f, 0.0f };
};