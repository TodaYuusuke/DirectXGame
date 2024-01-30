#pragma once
#include <Adapter.h>

#include "../Collision/Collider.h"
#include "PlayerBullet.h"

class Player : public Collider {
public: // メンバ関数

	// コンストラクタ
	Player();
	// デストラクタ
	~Player();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(LWP::Object::Camera* cameraPtr);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision() override;
	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	LWP::Math::Vector3 GetWorldPosition() override;

	/// <summary>
	/// 弾リストを取得
	/// </summary>
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	//*　公開する定数　*//

	// 当たり判定の半径
	const float kCollisionRadius = 3.0f;


private: // 関数

	/// <summary>
	/// 移動処理（キーボード）
	/// </summary>
	void MoveKeyboard();

	/// <summary>
	/// 移動処理（コントローラー）
	/// </summary>
	void MoveController();

	/// <summary>
	/// レティクルの狙っている座標を受け取る関数
	/// </summary>
	LWP::Math::Vector3 GetReticleTargetPosition();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Attack();


private: // メンバ変数

	//*　　定　数　　*//
	
	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.4f;
	// キャラクターの回転の速さ
	const float kRotSpeed = 0.02f;

	// 弾の速さ
	const float kBulletSpeed = 1.0f;
	// 射撃遅延時間
	const int kShotDelayTime = 10;


	//*　　変　数　　*//

	// モデル
	LWP::Primitive::Mesh* model_ = nullptr;
	// 2Dレティクル用スプライト
	LWP::Primitive::Sprite* reticle_ = nullptr;

	// カメラのポインタを保持しておく
	LWP::Object::Camera* parentCamera_ = nullptr;

	// 弾
	std::list<PlayerBullet*> bullets_;
	// 連続射撃制御用変数
	int shotDelay_ = 0;
};
