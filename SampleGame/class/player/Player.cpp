#include "Player.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;

// 初期化
void Player::Init(LWP::Object::Camera* ptr) {
	// カメラのポインタをセット
	camera_ = ptr;
	// ピストル初期化
	pistol_.Init(&camera_->transform);
}

// 更新
void Player::Update() {
	// 移動処理
	Move();
	// カメラの計算
	CameraRotate();

	// 加速度を加算
	//velocity.y -= kGravityAcce * Info::GetDeltaTimeF();
	// 速度を加算
	camera_->transform.translation += velocity;

	// 速度を減衰させる
	velocity.x *= kDecayRate;
	velocity.z *= kDecayRate;
	//// 前フレームで地形にヒットしていたなら重力加速度をリセット
	///*if (terrainPoint->preFrameHit) {
	//	velocity.y = 0.0f;
	//}*/

	// 銃更新
	pistol_.Update();
	
	// 射撃処理
	if (Keyboard::GetTrigger(DIK_SPACE) || Controller::GetTrigger(XBOX_RT)) {
		pistol_.Shot(
			camera_->transform.GetWorldPosition(),
			Vector3{ 0.0f,0.0f,1.0f } * camera_->transform.rotation
		);
	}
	// リロード
	if (Keyboard::GetTrigger(DIK_R) || Controller::GetTrigger(XBOX_X)) {
		pistol_.Reload();
	}

#if DEMO
	ImGui::Begin("Player");
	ImGui::DragFloat("kWalkSpeed", &kWalkSpeed, 0.1f);
	ImGui::DragFloat("kCameraRotationSpeed", &kCameraRotationSpeed, 0.1f);
	if (ImGui::TreeNode("Pistol")) {
		pistol_.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::End();
#endif
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
	//if (Keyboard::GetPress(DIK_LSHIFT)) {
	//	dir.y -= 1.0f;
	//}
	//if (Keyboard::GetPress(DIK_SPACE)) {
	//	dir.y += 1.0f;
	//}

	// コントローラーでの移動
	dir.x += Pad::GetLStick().x;
	dir.z += Pad::GetLStick().y;

	dir = Vector3(dir * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation));
	dir = Vector3{ dir.x, 0.0f, dir.z }.Normalize();
	// シフトを押しているならダッシュ
	if (Keyboard::GetPress(DIK_LSHIFT)) {
		velocity += (dir * 1.9f) * kWalkSpeed * Info::GetDeltaTimeF();
	}
	else {
		velocity += dir * kWalkSpeed * Info::GetDeltaTimeF();
	}
}


void Player::CameraRotate() {
	// 回転する向き
	Vector2 dir = { 0.0f,0.0f };

	// キーボードでの回転
	if (Keyboard::GetPress(DIK_UP)) {
		dir.x -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_DOWN)) {
		dir.x += 1.0f;
	}
	if (Keyboard::GetPress(DIK_RIGHT)) {
		dir.y += 1.0f;
	}
	if (Keyboard::GetPress(DIK_LEFT)) {
		dir.y -= 1.0f;
	}

	// コントローラーでの回転
	dir.x -= Pad::GetRStick().y;
	dir.y += Pad::GetRStick().x;

	// カメラを回転させる
	camera_->transform.rotation = 
		Quaternion::CreateFromAxisAngle(Vector3::UnitY(), dir.y * kCameraRotationSpeed)
		* camera_->transform.rotation *
		Quaternion::CreateFromAxisAngle(Vector3::UnitX(), dir.x * kCameraRotationSpeed);
}
