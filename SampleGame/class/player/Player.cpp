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
	//meshes[0].transform.rotation = Quaternion::ConvertEuler({ 0.0f,-1.57,0.0f });
	//meshes[0].transform.rotation = Quaternion({0.0f,-1.0f,0.0f}).Normalize();
	meshes[0].transform.scale = { 0.3f,0.3f,0.3f };
	meshes[1].LoadFile("Player/C_Head.gltf");
	meshes[2].LoadFile("Player/C_LHand.gltf");
	meshes[2].transform.translation.z = -1.0f;
	meshes[3].LoadFile("Player/C_RHand.gltf");
	meshes[3].transform.translation.z = 1.0f;
	// アニメーション用意
	idleAnim[0].LoadAnimationLongPath("resources/model/Player/C_Body.gltf", &meshes[0]);
	idleAnim[1].LoadAnimationLongPath("resources/model/Player/C_Head.gltf", &meshes[1]);
	idleAnim[2].LoadAnimationLongPath("resources/model/Player/C_LHand.gltf", &meshes[2]);
	idleAnim[3].LoadAnimationLongPath("resources/model/Player/C_RHand.gltf", &meshes[3]);

	meshes[1].transform.Parent(&meshes[0].transform);
	meshes[2].transform.Parent(&meshes[0].transform);
	meshes[3].transform.Parent(&meshes[0].transform);

	// カメラのポインタをセット
	camera_ = ptr;
	//camera_->transform.Parent(&meshes[0].transform);
	camera_->transform.translation = cameraOffset_;
	//camera_->transform.rotation = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, 1.54f);
	//camera_->transform.rotation *= Quaternion::ConvertEuler(Vector3{ 0.0f, -1.54f, 0.0f });
	//camera_->transform.rotation.x = 0.65f;
	//cameraGoalRotation_ = camera_->transform.rotation;

	// 点光源
	pl.transform.Parent(&meshes[0].transform);
	pl.transform.translation.x = 0.75f;
	pl.transform.translation.y = 1.0f;
	pl.radius = 13.0f;
	pl.intensity = 0.4f;
	pl.isActive = true;


	// まとめて行う処理
	for (int i = 0; i < 4; i++) {
		meshes[i].material.enableLighting = true;
		idleAnim[i].Start();
	}
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

	dir = Vector3(dir * camera_->transform.rotation).Normalize();
	meshes[0].transform.translation += dir * kPlayerSpeed;

	static Vector3 vec = Vector3{ 0.0f,-1.57f,0.0f };

	if (dir.Length() > 0.0f) {
		//meshes[0].transform.rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), std::atan2f(dir.z, dir.x)).Inverse();
	}

	ImGui::Begin("Test");
	ImGui::DragFloat3("vec", &vec.x);
	ImGui::End();

	// 今の方向
	//Vector3 startDir = Vector3{ 0.0f,0.0f,1.0f } *Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation);
	// 今の方向を向きたい方向にする回転行列を出す
	//Matrix4x4 rot = Matrix4x4::DirectionToDirection(startDir.Normalize(), dir3);
	// 回転行列をクォータニオンに変換
	//meshes[0].transform.rotation *= dir3;
	//meshes[0].transform.rotation = meshes[0].transform.rotation.Normalize();
	//meshes[0].transform.rotation = Utility::Interp::SlerpQuaternion(
	//	meshes[0].transform.rotation,
	//	Quaternion(dir3),
	//	0.8f
	//).Normalize();

	// 向きを設定
	if (dir.Length() > 0.0f) {
		//meshes[0].transform.rotation = Quaternion(dir3);
		// 現在の方向ベクトル
		//Math::Vector3 cuurentDir;
		//cuurentDir = Vector3{ 1.0f,0.0f,0.0f } * Matrix4x4::CreateRotateXYZMatrix(meshes[0].transform.rotation);
		//Vector3 temp = meshes[0].
		//
		//meshes->transform.rotation = meshes->transform.rotation * Matrix4x4::DirectionToDirection(cuurentDir,{ dir.x, 0.0f, dir.y });

		//prePos - meshes[0].transform.GetWorldPosition();

		// Runアニメーション
		for (int i = 0; i < 4; i++) {
			//idleAnim[i].Stop();
			//if (runAnim[i].isEnd()) { runAnim[i].Start(); }
		}
	}
	else {
		// 歩いていないのでIdleアニメーション
		for (int i = 0; i < 4; i++) {
			//if (idleAnim[i].isEnd()) { idleAnim[i].Start(); }
			//runAnim[i].Stop();
		}
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
	camera_->transform.translation = meshes[0].transform.GetWorldPosition() + cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation);
	// カメラから追従対象に対する角度を求める
	//camera_->transform.translation = meshes[0].transform.GetWorldPosition() + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(camera_->transform.rotation));
}
