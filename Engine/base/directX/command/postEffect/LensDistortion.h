#pragma once
#include <math/vector/Vector2.h>

/// <summary>
/// 歪曲収差
/// </summary>
class LensDistortion final {
public: // ** パラメータ ** //

	// アクティブフラグ
	bool isActive = false;

	// 強度（-100 ~ 100）
	int intensity = 0;
	// 影響度合い（0.0f ~ 1.0f）
	LWP::Math::Vector2 multiplier = { 1.0f,1.0f };
	// 歪みの中心（アンカーポイント）
	LWP::Math::Vector2 center = { 0.5f,0.5f };
	// レンズの拡大率（0.01f ~ 5.0f）
	float scale = 1.0f;
};