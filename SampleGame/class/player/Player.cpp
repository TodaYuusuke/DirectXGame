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
	//model.LoadShortPath("human/simpleSkin.gltf");
	model.worldTF.translation.y = 0.4f;
	//model.worldTF.scale = { 3.0f,3.0f,3.0f };
	model.worldTF.scale = { 0.4f,0.4f,0.4f };
	model.SetAllMaterialLighting(true);
	// アニメーション用意
	walkAnim.LoadAnimationLongPath("resources/model/human/walk.gltf", &model);
	//walkAnim.LoadAnimationLongPath("resources/model/human/simpleSkin.gltf", &model);
	
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
	pl.transform.translation.z = 2.0f;
	pl.transform.translation.y = 1.75f;
	pl.radius = 13.0f;
	pl.intensity = 0.7f;
	pl.isActive = true;

	walkAnim.Start();
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
	dir.x += Pad::GetLStick().x;
	dir.z += Pad::GetLStick().y;

	dir = Vector3(dir * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation)).Normalize();
	model.worldTF.translation += dir * kPlayerSpeed;

	if (dir.Length() > 0.0f) {
		// 現在プレイヤーの向いている角度
		Vector3 currentDir = Vector3{ 0.0f,0.0f,1.0f } * model.worldTF.rotation;
		currentDir;
		// 回転
		model.worldTF.rotation =
			Utility::Interp::SlerpQuaternion(
				model.worldTF.rotation,
				model.worldTF.rotation * Quaternion::DirectionToDirection(currentDir, dir), 0.8f);
		// 歩いているのでアニメーション
		if (walkAnim.isEnd()) {
			walkAnim.Start();
		}
	}
	else {
		// 止まってるのでアニメーションストップ
		walkAnim.Stop();
		walkAnim.Init();
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
	dir.x += Pad::GetRStick().y;
	dir.y += Pad::GetRStick().x;

	// 正規化してから使用
	dir = dir.Normalize();

	// カメラを回転させる
	camera_->transform.rotation *= Quaternion::CreateFromAxisAngle(Vector3{ 0.0f, 1.0f, 0.0f }, dir.y * kFollowCameraSpeed);
	//camera_->transform.rotation *= Quaternion::CreateFromAxisAngle(Vector3{ 1.0f, 0.0f, 0.0f } * camera_->transform.rotation, dir.x * kFollowCameraSpeed);
	// カメラの座標を決定
	camera_->transform.translation = model.worldTF.GetWorldPosition() + cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation);
	// カメラから追従対象に対する角度を求める
	//camera_->transform.translation = meshes[0].transform.GetWorldPosition() + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation));
}
