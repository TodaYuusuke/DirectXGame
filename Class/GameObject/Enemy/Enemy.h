#pragma once
#include <Adapter.h>

#include "EnemyState.h"
#include "EnemyBullet.h"

#include "../Collision/Collider.h"
#include "../../Utility/TimedCall/TimedCall.h"

class BaseEnemyState;
class GameScene;
class Player;

class Enemy : public Collider {

public: // メンバ関数
	// コンストラクタ
	Enemy() = default;
	// デストラクタ
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(LWP::Math::Vector3 position);

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

	// 指定した移動量座標を変更する関数
	void Move(LWP::Math::Vector3 speed) { model_->transform.translation += speed; }
	/// <summary>
	/// 座標を取得
	/// </summary>
	LWP::Math::Vector3 GetTranslation() { return model_->transform.translation; }
	// isAliveを受け取る
	bool isAlive() const { return isAlive_; }

	// 行動フェーズを変更する
	void ChangePhase(BaseEnemyState* newState);
	// 弾を発射してタイマーをリセットする関数
	void Fire();

	// プレイヤーとエネミーの距離を測る関数
	float GetEnemyToPlayerDistance();

	// ゲームシーンをセット
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// プレイヤーをセット
	void SetPlayer(Player* player) { player_ = player; }

public: // パブリックなメンバ変数
	
	//*　　定　数　　*//

	// 弾の発射間隔
	const int kFireInterval = 30;
	// 弾の速さ
	const float kBulletSpeed = 1.0f;

	// 当たり判定の半径
	const float kCollisionRadius = 3.0f;

	//*　　変　数　　*//

	// 発射処理を呼び出すコールバック
	TimedCall<void()>* timedCall_ = nullptr;

private: // メンバ変数

	// モデルの変数
	LWP::Primitive::Mesh* model_ = nullptr;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;
	// 現在の行動フェーズ
	BaseEnemyState* phase_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;

	// デスフラグ
	bool isAlive_ = true;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};
