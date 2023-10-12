#pragma once
#include "../IMob.h"

class DefenseObject final
	: public IMob {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// トランスフォームのポインタを返す関数
	/// </summary>
	LWP::Object::WorldTransform* GetTransform() { return &model_->transform; }

	/// <summary>
	/// Y軸を最高速度を受け取る
	/// </summary>
	float GetMaxSpeed() override { return 0.0f; }
};