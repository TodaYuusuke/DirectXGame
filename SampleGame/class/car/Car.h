#pragma once
#include <Adapter.h>

class Car final {
public:

	// 初期化
	void Init(LWP::Object::Camera* ptr, LWP::Object::Terrain* terrain);
	// 更新
	void Update();

private:

	// 重力加速度
	float kGravityAcce = 9.8f;
	// プレイヤーの加速度
	float kWalkSpeed = 6.0f;
	// 速度の減衰率
	float kDecayRate = 0.8f;

	// 3Dモデル
	LWP::Resource::EMapModel model;
	
	// 速度
	LWP::Math::Vector3 velocity = { 0.0f,0.0f,0.0f };
	// 地形とのコライダーポインタ
	LWP::Object::TransformQuat wtf[4];
	LWP::Object::Terrain::Point* frontPoint[2];
	LWP::Object::Terrain::Point* backPoint[2];

	void Move();


private: // ** カメラ系処理 ** // 
	
	// 追従カメラの速度
	const float kFollowCameraSpeed = 0.033f;

	// 追従させるカメラのポインタ
	LWP::Object::Camera* camera_ = nullptr;
	// カメラのオフセット
	LWP::Math::Vector3 cameraOffset_ = { 0.0f, 2.0f, -15.0f };

	void FollowCameraUpdate();
};