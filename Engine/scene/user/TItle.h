#pragma once
#include "../IScene.h"
#include <time.h>

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
	LWP::Primitive::Surface* surface;
	LWP::Primitive::Sphere* sphere;
	
	LWP::Primitive::Mesh* BoxModel;
	LWP::Primitive::Mesh* axisModel;

	LWP::Resource::Texture* uvTexture;
	LWP::Resource::Texture* monsterBall;

	bool useMonsterBall = true;

private: //**プライベートな関数**//
	// 攻撃
	void Attack();

	// タイマー系更新処理
	void UpdateTimer();

	// シーンチェンジ
	void SceneChange();

	// シェイク
	void Shake();

	// ボタンプッシュ
	void ButtonPush();

	//　イージング
	void Easing();

private: // ** ハンマー ** //

	// 攻撃クールタイム
	const int kAttackCoolTime = 60;
	int attackCoolTimer = -1;
	// モデル
	LWP::Primitive::Mesh* hammerModel = nullptr;

	bool selectPoint = true;

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
				{ 0.0f,-0.4f, 1.4f },
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
				{ 0.0f, 0.4f,-1.4f },
				{ 0.0f,0.0f,0.0f },
				{ 0.0f,0.0f,0.0f }
			};
		}swingUp;

		// 掛かる時間
		int GetFullTIme() { return swingDown.time + swingUp.time; }

		// アニメーション進行
		void Progress(LWP::Object::WorldTransform* transform);
	}attackAni;

	// ボタン押し込み
	struct Buttonanimation {
		// アニメーションタイマー
		int timer = -1;
		// 振り下ろし
		struct SwingDown {
			// 掛かる時間
			int time = 2;

			// 移動量
			LWP::Object::WorldTransform transform{
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f,0.0f,0.0f },
				{ 0.0f,0.0f,-0.3f }
			};
		}swingDown;
		// 戻す
		struct SwingUp {
			// 掛かる時間
			int time = 8;
			// 移動量
			LWP::Object::WorldTransform transform{
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f,0.0f,0.0f },
				{ 0.0f,0.0f,0.3f }
			};
		}swingUp;

		// 掛かる時間
		int GetFullTIme() { return swingDown.time + swingUp.time; }

		// アニメーション進行
		void Progress(LWP::Object::WorldTransform* transform);
	}buttonAni;

private: //****** タイム ***********//

	bool sceneChangeTiemFlag = false;

	int sceneChangeTime = 13;

private: //******* シェイク関連 ***********//
	int shakePosition = 0;
	
	int shakeMaxPosition = 0;

private: //******* ボタン ***************//
	
	LWP::Primitive::Mesh* buttonModel[2];

private: //********* イージング関連 ***********//
	bool isEasing = false;

};