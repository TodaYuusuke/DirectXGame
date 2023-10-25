#pragma once
#include "../IScene.h"
#include "../Class/Skydome/Skydome.h"

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

	// 地面
	LWP::Primitive::Surface* ground;

	LWP::Primitive::Triangle* tri[2];
	LWP::Primitive::Surface* TitleSurface;
	LWP::Primitive::Mesh* particleModel;
	
	LWP::Primitive::Mesh* BoxModel;
	LWP::Primitive::Mesh* axisModel;

	LWP::Resource::Texture* uvTexture;
	LWP::Resource::Texture* monsterBall;

	// SEデータ
	LWP::Resource::Audio* audio;

	bool useMonsterBall = true;

	// スカイドーム
	Skydome skydome_;

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

	// sceneTransition
	void SceneTransition();

	////　イージング
	//void Easing();

	//hammerの動き
	void moveHammer();

	// パーティクル
	//　作成
	void GenerateParticle(LWP::Math::Vector3& hitPosition);
	// 更新
	void UpdateParticles();

	// TitleUI 
	void TitleUI();

	void Controller();

private: // ** ハンマー ** //

	// 攻撃クールタイム
	const int kAttackCoolTime = 60;
	int attackCoolTimer = -1;
	// モデル
	LWP::Primitive::Mesh* hammerModel = nullptr;

	bool selectPoint = false;

	float moveSpeed = 0.3f;

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
				{ 0.0f,-0.4f, 0.0f },
				{ 1.2f,0.0f,0.0f },
				{ 0.0f,0.0f,0.0f }
			};
		}swingDown;
		// 戻す
		struct SwingUp {
			// 掛かる時間
			int time = 8;
			// 移動量
			LWP::Object::WorldTransform transform{
				{ 0.0f, 0.4f,0.0f },
				{ -1.2f,0.0f,0.0f },
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
				{ -0.2f,0.0f,-0.3f }
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
				{ 0.2f,0.0f,0.3f }
			};
		}swingUp;

		// 掛かる時間
		int GetFullTIme() { return swingDown.time + swingUp.time; }

		// アニメーション進行
		void Progress(LWP::Object::WorldTransform* transform);
	}buttonAni;

private: //******* シェイク関連 ***********//
	int shakePosition = 0;
	
	int shakeMaxPosition = 0;

private: //******* ボタン ***************//
	
	LWP::Primitive::Mesh* buttonModel[2];

	LWP::Primitive::Mesh* buttonFoundationModel[2];

private: //********* イージング関連 ***********//
	// イージング用ワールドトランスフォーム
	LWP::Object::WorldTransform hammerWorldTransform_[2];

	bool isEasing1 = false;
	bool isEasing2 = false;

	// イージング用タイム
	float easingTime[2] = { 0.0f };


private: //***********　シーン遷移用　***********//

	bool sceneChangeTiemFlag = false;

	int sceneChangeTime = 65;

	LWP::Primitive::Surface* transitionSurfase_[2] = { nullptr };

	// シーン遷移のフラグ(シーン遷移時にtrue)
	bool isTransition_ = false;
	bool isTransitionSceneEnd_ = false;

	// タイトルかリトライの選択ボタン
	bool isGoTitle = false;

private:// ****************** パーティクル ****************** //
	// パーティクルの構造体
	struct Particle {
		LWP::Math::Vector3 position;
		LWP::Math::Vector3 velocity;
		float gravity;
		float life;
	}particle;

	std::vector<Particle> particles;


private: // ** タイトルの関連 ** //
	LWP::Primitive::Mesh* tambourineModel[5];

	//ドンバリンUI
	LWP::Primitive::Surface* textDO;
	LWP::Primitive::Surface* textN[2];
	LWP::Primitive::Surface* textBA;
	LWP::Primitive::Surface* textRI;

};