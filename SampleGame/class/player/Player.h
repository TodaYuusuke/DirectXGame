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
	LWP::Math::Vector3 GetWorldPosition() { return camera_->worldTF.GetWorldPosition(); };


private: // ** メンバ定数 ** //

	// カメラの回転速度
	float kCameraRotationSpeed = 0.011f;


private: // ** メンバ変数 ** //
	
	// カメラのポインタ
	LWP::Object::Camera* camera_ = nullptr;

	// 銃のデータ
	Pistol pistol_;


private: // ** カメラ系処理 ** // 

	/// <summary>
	/// カメラ回転処理
	/// </summary>
	void CameraRotate();
};