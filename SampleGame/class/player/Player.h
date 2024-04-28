#pragma once
#include <Adapter.h>

class Player final {
public:

	// 初期化
	void Init(LWP::Object::Camera* ptr);
	// 更新
	void Update();

private:

	// プレイヤーの速度
	const float kPlayerSpeed = 0.1f;
	// 3Dモデル
	LWP::Primitive::Mesh meshes[4];
	// アニメーション
	LWP::Resource::Animation idleAnim[4];
	LWP::Resource::Animation runAnim[4];

	// クォータニオンの使い方がわからなかったので臨時でオイラー角で対応
	LWP::Math::Vector3 euler;
	void Move();

	// 光源
	LWP::Object::PointLight pl;


private: // ** カメラ系処理 ** // 
	
	// 追従カメラの速度
	const float kFollowCameraSpeed = 0.033f;

	// 追従させるカメラのポインタ
	LWP::Object::Camera* camera_ = nullptr;
	// カメラのオフセット
	//const LWP::Math::Vector3 cameraOffset_ = { -20.0f, 3.0f, 0.0f };
	const LWP::Math::Vector3 cameraOffset_ = { 0.0f, 3.0f, -20.0f };

	void FollowCameraUpdate();
};