#pragma once
#include "guns/Pistol.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player final {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(LWP::Object::Camera* ptr);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 座標を取得
	/// </summary>
	LWP::Math::Vector3 GetWorldPosition() { return camera_->transform.GetWorldPosition(); };


private: // ** メンバ定数 ** //

	// 重力加速度
	float kGravityAcce = 9.8f;
	// プレイヤーの加速度
	float kWalkSpeed = 3.0f;
	// 速度の減衰率
	float kDecayRate = 0.8f;

	// カメラの回転速度
	float kCameraRotationSpeed = 0.011f;


private: // ** メンバ変数 ** //
	
	// カメラのポインタ
	LWP::Object::Camera* camera_ = nullptr;
	// 速度
	LWP::Math::Vector3 velocity = { 0.0f,0.0f,0.0f };

	// 銃のデータ
	Pistol pistol_;


private: // ** カメラ系処理 ** // 

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
	/// <summary>
	/// カメラ回転処理
	/// </summary>
	void CameraRotate();
};