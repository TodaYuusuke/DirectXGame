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
	pistol_.Init(&camera_->worldTF);
}

// 更新
void Player::Update() {
	// カメラの計算
	CameraRotate();

	// 銃更新
	pistol_.Update();
	
	// 射撃処理
	if (Keyboard::GetTrigger(DIK_SPACE) || Controller::GetTrigger(XBOX_RT)) {
		pistol_.Shot(
			camera_->worldTF.GetWorldPosition(),
			Vector3{ 0.0f,0.0f,1.0f } * camera_->worldTF.rotation
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
	camera_->worldTF.rotation = 
		Quaternion::CreateFromAxisAngle(Vector3::UnitY(), dir.y * kCameraRotationSpeed)
		* camera_->worldTF.rotation *
		Quaternion::CreateFromAxisAngle(Vector3::UnitX(), dir.x * kCameraRotationSpeed);
}
