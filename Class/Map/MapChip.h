#pragma once
#include <Adapter.h>
#include "ShockWave.h"

class MapChip {
public:
	void Initialize();
	void Update();

	// 円の中かを判別する
	bool IsInCircle(Circle circle);

	// モデル
	LWP::Primitive::Mesh* model = nullptr;
	// デフォルトのY軸スケール
	float defaultScale;

	// マップが場外か判定
	bool isOut = true;

	// 自身の持つ波のパワー
	float wavePower = 0.0f;
	// 波のベクトル
	LWP::Math::Vector2 waveVec = { 0.0f, 0.0f };

	// 衝撃波に対してヒット済みかどうかを保持する変数
	bool isHit[3];
};