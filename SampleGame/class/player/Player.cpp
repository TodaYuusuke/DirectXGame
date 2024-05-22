#include "Player.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;

// 初期化
void Player::Init(LWP::Object::Camera* ptr) {
	// モデル用意
	model.LoadShortPath("human/walk.gltf");
	model.worldTF.scale = { 0.3f,0.3f,0.3f };
	// アニメーション用意
	//walkAnim.LoadAnimationLongPath("resources/model/Player/C_Body.gltf", &model);
	
	// カメラのポインタをセット
	camera_ = ptr;
	//camera_->transform.Parent(&meshes[0].transform);
	camera_->transform.translation = cameraOffset_;
	//camera_->transform.rotation = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, 1.54f);
	//camera_->transform.rotation *= Quaternion::ConvertEuler(Vector3{ 0.0f, -1.54f, 0.0f });
	//camera_->transform.rotation.x = 0.65f;
	//cameraGoalRotation_ = camera_->transform.rotation;

	// 点光源
	pl.transform.Parent(&model.worldTF);
	pl.transform.translation.x = 0.75f;
	pl.transform.translation.y = 1.0f;
	pl.radius = 13.0f;
	pl.intensity = 0.4f;
	pl.isActive = true;


	// まとめて行う処理
	//for (int i = 0; i < 4; i++) {
	//	meshes[i].material.enableLighting = true;
	//walkAnim.Start();
	//}
}

// 更新
void Player::Update() {
	// 移動処理
	Move();
	// カメラの計算
	FollowCameraUpdate();
}

void Player::Move() {
	// 移動する向き
	Vector3 dir = { 0.0f,0.0f,0.0f };

	// キーボードでの移動
	if (Keyboard::GetPress(DIK_W)) {
		dir.z += 1.0f;
	}
	if (Keyboard::GetPress(DIK_S)) {
		dir.z -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_D)) {
		dir.x += 1.0f;
	}
	if (Keyboard::GetPress(DIK_A)) {
		dir.x -= 1.0f;
	}

	// コントローラーでの移動
	dir.x += Pad::GetLStick(0).x;
	dir.z += Pad::GetLStick(0).y;

	dir = Vector3(dir * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation)).Normalize();
	model.worldTF.translation += dir * kPlayerSpeed;

	if (dir.Length() > 0.0f) {
		// オイラー角
		Vector3& rotation = model.worldTF.rotation;

		// 目的の角度
		Vector3 goalRotation = { 0.0f, 0.0f, 0.0f };
		// Y軸周りの角度
		goalRotation.y = std::atan2f(dir.x, dir.z);
		// モデルの回転分補正
		goalRotation.y -= 1.57f;
		// X軸周りの角度
		//goalRotation.x = std::atan2f(-dir.y, Vector3{ dir.x, 0.0f, dir.z }.Length());

		// 現在の角度と目標の角度を比較し、逆回転の場合に調整
		if (std::abs(rotation.y - goalRotation.y) > M_PI) {
			if (rotation.y > goalRotation.y) {
				rotation.y -= static_cast<float>(2.0f * M_PI);
			}
			else {
				rotation.y += static_cast<float>(2.0f * M_PI);
			}
		}

		// 回転適応
		rotation.y = goalRotation.y;
		//rotation = Utility::Interp::Slerp(rotation, goalRotation, 0.2f);
	}
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

	// 正規化してから使用
	dir = dir.Normalize();

	// 移動量のクォータニオン
	Quaternion quat = Quaternion::ConvertEuler(Vector3{ 0.0f,dir.y, 0.0f } *kFollowCameraSpeed);
	// カメラを回転させる
	camera_->transform.rotation *= quat;
	// カメラの座標を決定
	camera_->transform.translation = model.worldTF.GetWorldPosition() + cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation);
	// カメラから追従対象に対する角度を求める
	//camera_->transform.translation = meshes[0].transform.GetWorldPosition() + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation));
}
