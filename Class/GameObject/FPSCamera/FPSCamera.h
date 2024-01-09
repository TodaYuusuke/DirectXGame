#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

class FPSCamera {
public: // メンバ関数
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 追従対象をセット
	/// </summary>
	void SetTarget(const WorldTransform* target) { target_ = target; }
	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	const ViewProjection& GetViewProjection() { return viewProjection_; }


private: // メンバ変数
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// ターゲット
	const WorldTransform* target_;
	// カメラの角度
	Vector3 rotation_;
};