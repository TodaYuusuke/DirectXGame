#pragma once
#include <Adapter.h>

class HitEffect {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// エフェクト開始
	/// </summary>
	bool Start(LWP::Math::Vector3 position);

private:

	// 開始済みフラグ
	bool isStarted = false;
	// 開始からの経過フレーム
	const static int kEffectFrame = 20;
	int elapsedFrame = 0;

	// 球体
	const static int kSphereAmount = 16;
	LWP::Primitive::Sphere* spheres[kSphereAmount];

	// 速度ベクトル
	LWP::Math::Vector3 velocity[kSphereAmount];
};