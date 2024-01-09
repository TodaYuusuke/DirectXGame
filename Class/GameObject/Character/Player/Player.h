#pragma once
#include "../BaseCharacter.h"
#include "../../Bullet/Bullet.h"
#include "../../Gun/HandGun.h"
#include <optional>
#define _USE_MATH_DEFINES
#include <math.h>

class Player : public BaseCharacter {
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
	void Initialize(const std::vector<Model*>& models, Vector3 position) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection) override;
	void DrawUI();

	// 弾リストを返す関数
	const std::list<Bullet*>& GetBullets() const { return bullets_; }

	// 衝突時に呼ばれる関数
	void OnCollision() override {};

private: // メンバ定数
	// 移動速度
	const float kCharacterSpeed = 0.3f;
	// カメラ速度
	const float kCameraTurnSpeed = 0.02f;
	// 弾の速さ
	const float kBulletSpeed = 5.0f;

private: // メンバ変数

	bool preRS = false;	// RSTrigger検知用
	bool preLS = false; // LSTrigger検知用
	bool preX = true; // XTrigger検知用

	// 弾丸のモデル
	std::unique_ptr<Model> bulletModel_;
	// 弾丸
	std::list<Bullet*> bullets_;

	// 受け取ったリコイル
	Vector3 recoil_ = {0.0f, 0.0f, 0.0f};
	// 本来の向き
	Vector3 rotation_ = {0.0f, 0.0f, 0.0f};

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
