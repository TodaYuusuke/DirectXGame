#pragma once
#include "HitEffect.h"

class EffectManager {
public:
	/// <summary>
	/// シングルトン
	/// </summary>
	static EffectManager* GetInstance();

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
	void Start(LWP::Math::Vector3 position);

private:
	EffectManager() = default;
	~EffectManager() = default;
	EffectManager(const EffectManager&) = delete;
	const EffectManager& operator=(const EffectManager&) = delete;


	const static int kEffectAmount = 5;
	HitEffect effect[kEffectAmount];
};