#pragma once
#include <Adapter.h>

/// <summary>
/// ムービー中の黒帯
/// </summary>
class BlackBelt final {
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Init();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// アクティブフラグ切り替え
	/// </summary>
	void SetIsActive(bool b) { isActive_ = b; }

private: // ** メンバ変数 ** //
	// 上下の黒帯用スプライト
	LWP::Primitive::NormalSprite blackBelt_[2];

	// 黒帯の縦のスケール
	float scale_ = 0.7f;
	// アニメーションタイム
	float time_ = 0.0f;

	// アクティブフラグ
	bool isActive_ = false;
};