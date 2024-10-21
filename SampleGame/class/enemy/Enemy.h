#pragma once
#include "class/player/Player.h"
#include "BloodParticle.h"

/// <summary>
/// 敵クラス
/// </summary>
class Enemy final {
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(LWP::Utility::CatmullRom* curve, Player* player, BloodParticle* particle);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 死亡した上でアニメーションが終了したかを返す
	/// </summary>
	bool GetIsDead() { return isDead_; }

	/// <summary>
	/// Debug用のImGui
	/// </summary>
	void DebugGUI();


private: // ** メンバ変数 ** //
	// 保持するポインタ
	Player* player_;
	BloodParticle* particle_;
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
	
	// 死亡アニメーション後の消えるまでの時間
	float deathTime_ = 0.0f;
	// 死亡フラグ
	bool isDeath_ = false;
	// 死亡演出まで終了フラグ
	bool isDead_ = false;


private: // ** プライベートなメンバ関数 ** //

	/// <summary>
	/// 死亡演出まとめ
	/// </summary>
	void DeathAnimation();
};