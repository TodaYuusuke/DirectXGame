#pragma once
#include <Adapter.h>

// 円を示す構造体
struct Circle {
	// 中心点
	LWP::Math::Vector2 center{};
	// 半径
	float radius{};
};

class ShockWave {
public:
	void Initialize(LWP::Math::Vector2 pos);
	void Update();

	// アクティブかどうか
	bool isActive = false;

	// 自身の円
	Circle circle;
	// 半径の広がる速度
	float speed;
	// 半径の最大サイズ
	float kMaxRadius = 0.7f;
};