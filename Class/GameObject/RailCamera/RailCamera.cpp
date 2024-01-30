#include "RailCamera.h"

using namespace LWP::Math;

void RailCamera::Initialize(LWP::Math::Vector3 worldPosition, LWP::Math::Vector3 radian) {
	// インスタンスを作成
	camera_ = LWP::Object::CreateInstance<LWP::Object::Camera>();
	
	// ワールドトランスフォームの初期化
	camera_->transform.translation = worldPosition;
	camera_->transform.rotation = radian;

	// 制御点をセット
	catmullRom_.controlPoints = {
		{ 0,  0,  0},
		{ 3, 10, 10},
		{ 6, 15, 10},
		{ 9, 15, 20},
		{ 6,  0, 20},
		{ 3,  0, 30},
		{ 0,  0, 40},
		{ 0,  0, 50},
		{ 0,  0, 60},
		{ 0,  0, 70},
	};
}

void RailCamera::Update() {
	// レールに沿って進む
	catmullRom_.t += 0.001f;
	catmullRom_.t = std::clamp(catmullRom_.t, 0.0f, 1.0f);
	camera_->transform.translation = catmullRom_.GetPosition();

	// 回転角を求める
	float forwordT = catmullRom_.t + 0.1f;
	forwordT = std::clamp(forwordT, 0.0f, 1.0f);
	Vector3 forword = catmullRom_.GetPosition(forwordT) - camera_->transform.translation;
	// Y軸周りの角度
	camera_->transform.rotation.y = std::atan2f(forword.x, forword.z);
	// X軸周りの角度
	camera_->transform.rotation.x = std::atan2f(-forword.y, Vector3{ forword.x, 0, forword.z }.Length());
}