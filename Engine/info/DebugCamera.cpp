#include "DebugCamera.h"
#include "Adapter.h"

using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Information;
using namespace LWP::Utility;

void DebugCamera::Init() {
	camera_.isActive = false;
	camera_.name = "DebugCamera";
}
void DebugCamera::Update() {
	// 早期リターン
	if (!camera_.isActive) { return; }

	// 移動処理
	Move();
	Rotate();
}

void DebugCamera::Enable() {
	cameraPtr_ = Object::Manager::GetInstance()->GetMainCamera();
	camera_.worldTF.translation = cameraPtr_->worldTF.GetWorldPosition();
	camera_.worldTF.rotation = cameraPtr_->worldTF.rotation;
	camera_.worldTF.scale = cameraPtr_->worldTF.scale;
	camera_.isActive = true;
	camera_.SetMainCamera();
}
void DebugCamera::Disable() {
	cameraPtr_->SetMainCamera();
	cameraPtr_ = nullptr;
	camera_.isActive = false;
}
void DebugCamera::ToggleEnable() {
	// オフならオン
	if (!camera_.isActive) { Enable(); }
	// オンならオフ
	else { Disable(); }
}

void DebugCamera::ApplyTransform() {
	cameraPtr_->worldTF = camera_.worldTF;
}

void DebugCamera::DebugGUI() {
	if (ImGui::BeginTabItem("Info")) {
		if (ImGui::TreeNode("Debug Camera")) {
			ImGui::Text("WASD : Move  LControl : Dash(Speed 2x)");
			ImGui::Text("Space : Up  LShift : Down");
			ImGui::Text("Arrow Key : Rotate");
			// オンボタン
			if (!camera_.isActive) {
				if (ImGui::Button("Enable")) { Enable(); }
			}
			// オフボタン
			else {
				if (ImGui::Button("Disable")) { Disable(); }
				if (ImGui::Button("Apply Transform")) { ApplyTransform(); }
				ImGui::Text("-------------");
				ImGui::DragFloat("Speed", &cameraSpeed_, 0.01f);	// カメラの移動速度
				ImGui::DragFloat("Rotate Speed", &cameraRotateSpeed_, 0.001f);	// カメラの回転速度
				if (ImGui::TreeNode("Camera")) {
					camera_.DebugGUI();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::EndTabItem();
	}
}

void DebugCamera::Move() {
	// 移動する向き
	Vector3 dir = { 0.0f,0.0f,0.0f };

	// キーボードでの移動
	if (Keyboard::GetPress(DIK_W)) { dir.z += 1.0f; }
	if (Keyboard::GetPress(DIK_S)) { dir.z -= 1.0f; }
	if (Keyboard::GetPress(DIK_D)) { dir.x += 1.0f; }
	if (Keyboard::GetPress(DIK_A)) { dir.x -= 1.0f; }
	if (Keyboard::GetPress(DIK_SPACE)) { dir.y += 1.0f; }
	if (Keyboard::GetPress(DIK_LSHIFT)) { dir.y -= 1.0f; }

	dir = Vector3(dir * Matrix4x4::CreateRotateXYZMatrix(camera_.worldTF.rotation));
	dir = dir.Normalize();
	// コントロールキーを押しているなら2倍速で走る
	if (Keyboard::GetPress(DIK_LCONTROL)) { dir *= 2.0f; }

	// 加算
	camera_.worldTF.translation += dir * cameraSpeed_ * Info::GetDeltaTimeF();
}
void DebugCamera::Rotate() {
	// 回転する向き
	Vector2 dir = { 0.0f,0.0f };

	// キーボードでの回転
	if (Keyboard::GetPress(DIK_UP)) { dir.x -= 1.0f; }
	if (Keyboard::GetPress(DIK_DOWN)) { dir.x += 1.0f; }
	if (Keyboard::GetPress(DIK_RIGHT)) { dir.y += 1.0f; }
	if (Keyboard::GetPress(DIK_LEFT)) { dir.y -= 1.0f; }

	// カメラを回転させる
	camera_.worldTF.rotation =
		Quaternion::CreateFromAxisAngle(Vector3::UnitY(), dir.y * cameraRotateSpeed_)
		* camera_.worldTF.rotation *
		Quaternion::CreateFromAxisAngle(Vector3::UnitX(), dir.x * cameraRotateSpeed_);
}