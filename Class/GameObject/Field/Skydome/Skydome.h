#pragma once
#include "WorldTransform.h"
#include "Model.h"

/// <summary>
/// 天球
/// </summary>
class Skydome {
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjecttion_);

private: // メンバ変数

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};