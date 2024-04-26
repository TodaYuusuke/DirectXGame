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
	LWP::Resource::Animation anim[4];

	// 光源
	LWP::Object::PointLight pl;


private: // ** カメラ系処理 ** // 
	
	// 追従カメラの速度
	const float kFollowCameraSpeed = 0.033f;

	// 追従させるカメラのポインタ
	LWP::Object::Camera* camera_ = nullptr;
	// 追従カメラのゴール地点
	LWP::Math::Vector3 cameraGoalTranslation_ = { 0.0f, 0.0f, 0.0f };
	LWP::Math::Vector3 cameraGoalRotation_ = { 0.0f, 0.0f, 0.0f };
	// カメラのオフセット
	LWP::Math::Vector3 cameraOffset_ = { 0.0f, 0.0f, -8.0f };

	void FollowCameraUpdate();
};