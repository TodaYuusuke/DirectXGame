#include "Player.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;

// 初期化
void Player::Init(LWP::Object::Camera* ptr) {
	// モデル用意
	meshes[0].LoadFile("Player/C_Body.gltf");
	meshes[1].LoadFile("Player/C_Head.gltf");
	meshes[2].LoadFile("Player/C_LHand.gltf");
	meshes[2].LoadFile("Player/C_RHand.gltf");
	// アニメーション用意
	anim[0].LoadAnimationLongPath("resources/model/Player/C_Body.gltf", &meshes[0]);
	anim[1].LoadAnimationLongPath("resources/model/Player/C_Head.gltf", &meshes[1]);
	anim[2].LoadAnimationLongPath("resources/model/Player/C_LHand.gltf", &meshes[2]);
	anim[3].LoadAnimationLongPath("resources/model/Player/C_RHand.gltf", &meshes[3]);

	meshes[1].transform.Parent(&meshes[0].transform);
	meshes[2].transform.Parent(&meshes[0].transform);
	meshes[3].transform.Parent(&meshes[0].transform);

	// カメラのポインタをセット
	camera_ = ptr;
	camera_->transform.rotation.x = 0.65f;
	cameraGoalRotation_ = camera_->transform.rotation;
	camera_->transform.translation = meshes[0].transform.translation + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(cameraGoalRotation_));
	cameraGoalTranslation_ = camera_->transform.translation;

	// 点光源
	pl.transform.Parent(&meshes[0].transform);
	pl.transform.translation.z = 0.1f;
	pl.radius = 13.0f;
	pl.intensity = 0.4f;
	pl.isActive = true;
}

// 更新
void Player::Update() {
	// 移動
	Math::Vector2 dir{};
	if (Keyboard::GetPress(DIK_W)) {
		dir.y += 1.0f;
	}
	if (Keyboard::GetPress(DIK_S)) {
		dir.y -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_A)) {
		dir.x -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_D)) {
		dir.x += 1.0f;
	}

	dir = dir.Normalize();
	meshes[0].transform.translation += Math::Vector3{ dir.x, 0.0f, dir.y } * kPlayerSpeed;

	// カメラの計算
	FollowCameraUpdate();
}

void Player::FollowCameraUpdate() {
	// 回転する向き
	Vector2 dir = { 0.0f,0.0f };

	// キーボードでの回転
	if (Keyboard::GetPress(DIK_UP)) {
		dir.x += 1.0f;
	}
	if (Keyboard::GetPress(DIK_DOWN)) {
		dir.x -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_RIGHT)) {
		dir.y += 1.0f;
	}
	if (Keyboard::GetPress(DIK_LEFT)) {
		dir.y -= 1.0f;
	}

	// コントローラーでの回転
	dir.x += Pad::GetRStick(0).y;
	dir.y += Pad::GetRStick(0).x;

	// 長さが1を超えている場合 -> 正規化してから使用
	dir = dir.Length() > 1.0f ? dir.Normalize() : dir;
	// 次の目標回転角を計算
	cameraGoalRotation_ += Vector3{ dir.x * kFollowCameraSpeed, dir.y * kFollowCameraSpeed, 0.0f };
	// 角度制限
	cameraGoalRotation_.x = std::clamp<float>(cameraGoalRotation_.x, 0.1f, 1.3f);

	// カメラから追従対象に対する角度を求める
	cameraGoalTranslation_ = meshes[0].transform.translation + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(cameraGoalRotation_));

	// 徐々に追従するように適応
	camera_->transform.translation = Utility::Interp::Lerp(camera_->transform.translation, cameraGoalTranslation_, 0.07f);
	camera_->transform.rotation = Utility::Interp::Lerp(camera_->transform.rotation, cameraGoalRotation_, 0.06f);
}
