#pragma once
#include <Adapter.h>

/// <summary>
/// 天球
/// </summary>
class Skydome {
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private: // メンバ変数

	// モデル
	LWP::Primitive::Mesh* model_ = nullptr;
};