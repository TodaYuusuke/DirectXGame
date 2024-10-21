#pragma once
#include "class/player/Player.h"

/// <summary>
/// 敵クラス
/// </summary>
class Enemy final {
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(LWP::Utility::CatmullRom* curve, Player* player);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 死亡したかフラグ
	/// </summary>
	bool GetIsDead() { return isDead_; }

	/// <summary>
	/// Debug用のImGui
	/// </summary>
	void DebugGUI();


private: // ** メンバ変数 ** //
	// プレイヤーの情報を貰う
	Player* player_;
	// 自分が追従するカーブのデータ
	LWP::Utility::CatmullRom* curve_;

	// モデル
	LWP::Resource::SkinningModel model_;
	// アニメーション
	LWP::Resource::Animation anim_;
	// コライダー
	LWP::Object::Collision collision_;

	// 体力
	int health_ = 1;
	// 死亡フラグ
	bool isDead_ = false;
};