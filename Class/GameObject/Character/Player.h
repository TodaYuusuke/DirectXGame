#pragma once
#include "../Collision/Collider.h"
#include "../Bullet/Bullet.h"
#include "../Gun/HandGun.h"
#include <optional>
#define _USE_MATH_DEFINES
#include <math.h>

class Player : public Collider {
public: // メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(LWP::Math::Vector3 position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	// 弾リストを返す関数
	const std::list<Bullet*>& GetBullets() const { return bullets_; }

	// 衝突時に呼ばれる関数
	void OnCollision() override {};

	// プレイヤーのtransformを受け取る関数
	LWP::Object::WorldTransform* GetTransform() { return &transform_; }

private: // メンバ定数
	// 移動速度
	const float kCharacterSpeed = 0.3f;
	// カメラ速度
	const float kCameraTurnSpeed = 0.02f;
	// 弾の速さ
	const float kBulletSpeed = 5.0f;

private: // メンバ変数

	// 自分の座標
	LWP::Object::WorldTransform transform_;

	// 弾丸
	std::list<Bullet*> bullets_;

	// 受け取ったリコイル
	LWP::Math::Vector3 recoil_ = {0.0f, 0.0f, 0.0f};
	// 本来の向き
	LWP::Math::Vector3 rotation_ = {0.0f, 0.0f, 0.0f};

	// ハンドガン
	std::unique_ptr<HandGun> handgun_;

private: // プライベートな関数

	// コントローラーでの操作
	void MoveJoyStick();
	// 弾丸の更新
	void UpdateBullets();

	// モデルを初期値に戻す
	void SetModelNeutral();
	// 調整項目適応
	void ApplyGlobalVariables();
};
