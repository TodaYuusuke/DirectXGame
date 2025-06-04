#pragma once
#include "class/player/Player.h"

/// <summary>
/// 敵クラス
/// </summary>
class Enemy final {
public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy(float speed) : kSpeed(speed) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(LWP::Utility::CatmullRom* curve, Player* player, std::function<void(LWP::Math::Vector3)> func);
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


private: // ** メンバ定数 ** //

	// 移動速度
	float kSpeed;

private: // ** メンバ変数 ** //
	// 保持するポインタ
	Player* player_ = nullptr;
	std::function<void(LWP::Math::Vector3)> particleFunc_;
	// 自分が追従するカーブのデータ
	LWP::Utility::CatmullRom* curve_ = nullptr;

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