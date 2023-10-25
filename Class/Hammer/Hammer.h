#pragma once
#include <Adapter.h>
#include "../Map/MapManager.h"
#include "../Class/Tambourine/Tambourine.h"

class Hammer final {
public: // ** メンバ関数 ** //

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(MapManager* map);
	/// <summary>
	/// 初期化
	/// </summary>
	void Update();

	const LWP::Object::WorldTransform GetTransform() { return model_->transform.translation; }

private: // ** メンバ変数 ** //

	// 移動速度
	const float kReticleSpeed = 0.02f;
	
	// 攻撃クールタイム
	const int kAttackCoolTime = 60;
	int attackCoolTimer = -1;

	// サウンド
	LWP::Resource::Audio* impactSE_;
	LWP::Resource::Audio* bellSE_;

	// モデル
	LWP::Primitive::Mesh* model_ = nullptr;
	// タンバリン
	Tambourine tambourine_;
	// 打撃地点のレティクル
	LWP::Primitive::Surface* reticle_ = nullptr;

	// マップのポインタ
	MapManager* mapPtr_ = nullptr;

	//　コントローラーのLStickのvelocity
	LWP::Math::Vector2 LStickVelocity;

	float centerX;
	float centerY;
	float radius;

	float deltaX;
	float deltaY;

private: // ** プライベートな関数 ** //

	// タイマー系更新処理
	void UpdateTimer();

	// キーボード操作
	void keyBoard();
	// コントローラー操作
	void Controller();
	
	// 攻撃
	void Attack();

private: // ** アニメーション関連 ** //

	// ハンマー振り下ろし
	struct AttackAnimation {
		// アニメーションタイマー
		int timer = -1;
		// 振り下ろし
		struct SwingDown {
			// 掛かる時間
			int time = 2;
			// 移動量
			LWP::Object::WorldTransform transform{
				{ 0.0f,-0.99f,0.0f },
				{ 0.0f,0.0f,0.0f },
				{ 0.0f,0.0f,0.0f }
			};
		}swingDown;
		// 戻す
		struct SwingUp {
			// 掛かる時間
			int time = 8;
			// 移動量
			LWP::Object::WorldTransform transform{
				{ 0.0f,0.99f,0.0f },
				{ 0.0f,0.0f,0.0f },
				{ 0.0f,0.0f,0.0f }
			};
		}swingUp;

		// 掛かる時間
		int GetFullTIme() { return swingDown.time + swingUp.time; }

		// アニメーション進行
		void Progress(LWP::Object::WorldTransform* transform);
	}attackAni;
};