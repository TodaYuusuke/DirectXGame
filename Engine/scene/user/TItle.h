#pragma once
#include "../IScene.h"

class Title final
	: public IScene {
public:
	Title() = default;
	~Title() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Primitive::Triangle* tri[2];
	LWP::Primitive::Surface* surface[3];
	LWP::Primitive::Sphere* sphere;
	
	LWP::Primitive::Mesh* cubeModel;
	LWP::Primitive::Mesh* axisModel;

	LWP::Resource::Texture* uvTexture;
	LWP::Resource::Texture* monsterBall;

	bool useMonsterBall = true;

private: //**プライベートな関数**//
	// 攻撃
	void Attack();

	// タイマー系更新処理
	void UpdateTimer();

private: // ** ハンマー ** //

	// 攻撃クールタイム
	const int kAttackCoolTime = 60;
	int attackCoolTimer = -1;
	// モデル
	LWP::Primitive::Mesh* hammerModel = nullptr;


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
				{ 0.0f,0.0f, 2.0f },
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
				{ 0.0f, 0.0f,-2.0f },
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