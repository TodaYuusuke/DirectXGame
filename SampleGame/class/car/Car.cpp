#include "Car.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;

// 初期化
void Car::Init(LWP::Object::Camera* ptr, LWP::Object::Terrain* terrain) {
	model.LoadShortPath("cars/TestCar.gltf");
	model.materials[0].enableLighting = false;
	model.materials[1].enableLighting = false;
	model.materials[2].shininess = 200;
	model.materials[4].enableLighting = false;
	model.materials[5].enableLighting = false;
	model.materials[6].enableLighting = false;
	
	// カメラのポインタをセット
	camera_ = ptr;
	camera_->transform.translation = cameraOffset_;

	// 地形に接地部分を登録
	for (int i = 1; i < 5; i++) {
		wtf[i].Parent(&wtf[0]);
	}
	wtf[1].translation = { -0.68f,0.0f,1.24f };
	wtf[2].translation = {  0.68f,0.0f,1.24f };
	wtf[3].translation = { -0.68f,0.0f,-1.24f };
	wtf[4].translation = {  0.68f,0.0f,-1.24f };

	terrain->SetNewCollider({ 0.0f,0.0f,0.0f }, &model.worldTF);
	frontPoint[0] = terrain->SetNewCollider({ 0.0f,0.0f,0.0f }, &wtf[1]);
	frontPoint[1] = terrain->SetNewCollider({ 0.0f,0.0f,0.0f }, &wtf[2]);
	backPoint[0] = terrain->SetNewCollider({ 0.0f,0.0f,0.0f }, &wtf[3]);
	backPoint[1] = terrain->SetNewCollider({ 0.0f,0.0f,0.0f }, &wtf[4]);
}

// 更新
void Car::Update() {
	// 車の現在地を求める
	//Vector3 currentPos = { 0.0f,0.0f,0.0f };
	//for (int i = 1; i < 5; i++) {
	//	currentPos += wtf[i].GetWorldPosition();
	//	wtf[i].translation.y = 0.0f;
	//}
	//model.worldTF.translation.y = currentPos.y / 4.0f;

	// 移動処理
	Move();
	// カメラの計算
	FollowCameraUpdate();

	// 速度を加算
	model.worldTF.translation += velocity;

	wtf[0] = model.worldTF;
	wtf[0].translation.y = 0.0f;

	// 速度を減衰させる
	velocity.x *= kDecayRate;
	velocity.z *= kDecayRate;
	// 前フレームで地形にヒットしていたなら重力加速度をリセット
	if (frontPoint[0]->preFrameHit || frontPoint[1]->preFrameHit || backPoint[0]->preFrameHit || backPoint[1]->preFrameHit) {
		velocity.y = 0.0f;
	}

#if DEMO
	ImGui::Begin("Car");
	if (ImGui::TreeNode("Model")) {
		model.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::DragFloat("kSpeed", &kWalkSpeed, 0.1f);
	ImGui::DragFloat3("CameraOffset", &cameraOffset_.x, 0.1f);
	for (int i = 0; i < 4; i++) {
		if (ImGui::TreeNode(("wheel" + std::to_string(i)).c_str())) {
			wtf[i].DebugGUI();
			Vector3 p = wtf[i].GetWorldPosition();
			ImGui::Text("%f, %f, %f", p.x, p.y, p.z);
			ImGui::TreePop();
		}
	}
	//ImGui::DragFloat3("centerPosition", &currentPos.x);
	ImGui::End();
#endif
}

void Car::Move() {
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
	dir = Vector3{ dir.x, 0.0f, dir.z }.Normalize();
	// シフトを押しているならダッシュ
	if (Keyboard::GetPress(DIK_LSHIFT)) {
		velocity += (dir * 1.9f) * kWalkSpeed * Info::GetDeltaTimeF();
	}
	else {
		velocity += dir * kWalkSpeed * Info::GetDeltaTimeF();
	}

	if (dir.Length() > 0.0f) {
		// 現在プレイヤーの向いている角度
		Vector3 currentDir = Vector3{ 0.0f,0.0f,1.0f } * model.worldTF.rotation;
		currentDir = Vector3{ currentDir.x, 0.0f, currentDir.z }.Normalize();
		// 回転
		model.worldTF.rotation =
			Utility::Interp::SlerpQuaternion(
				model.worldTF.rotation,
				model.worldTF.rotation * Quaternion::DirectionToDirection(currentDir, dir), 0.8f);
	}
}


void Car::FollowCameraUpdate() {
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
	camera_->transform.rotation = 
		Quaternion::CreateFromAxisAngle(Vector3::UnitY(), dir.y * kFollowCameraSpeed)
		* camera_->transform.rotation *
		Quaternion::CreateFromAxisAngle(Vector3::UnitX(), dir.x * kFollowCameraSpeed);
	//camera_->transform.rotation *= Quaternion::CreateFromAxisAngle(Vector3{ 1.0f, 0.0f, 0.0f } * camera_->transform.rotation, dir.x * kFollowCameraSpeed);
	// カメラの座標を決定
	camera_->transform.translation = model.worldTF.GetWorldPosition() + cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation);
	// カメラから追従対象に対する角度を求める
	//camera_->transform.translation = meshes[0].transform.GetWorldPosition() + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation));
}
