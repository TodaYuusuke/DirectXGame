#pragma once
#include "../IMob.h"

class Enemy final
	: public IMob {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 地上にいる際の処理
	/// </summary>
	void Moving() override;

	/// <summary>
	/// ターゲットのポインタを受け取る関数
	/// </summary>
	void SetTarget(LWP::Object::WorldTransform* target) { target_ = target; }

private: // ** メンバ定数 ** //

	// 加速度
	const float kAccele = 0.00002f;

private: // ** メンバ変数 ** //

	// 護衛対象の座標のポインタ
	LWP::Object::WorldTransform* target_ = nullptr;
};