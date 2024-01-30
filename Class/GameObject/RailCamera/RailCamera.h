#pragma once
#include <Adapter.h>

class RailCamera {
public: // メンバ関数
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(LWP::Math::Vector3 worldPosition, LWP::Math::Vector3 radian);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// カメラのインスタンスを取得
	/// </summary>
	/// <returns></returns>
	LWP::Object::Camera* GetCamera() { return camera_; }

private: // メンバ変数
	
	// カメラのインスタンス
	LWP::Object::Camera* camera_ = nullptr;

	// スプライン曲線制御
	LWP::Utility::CatmullRom catmullRom_;
};