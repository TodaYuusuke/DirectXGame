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
	meshes[0].transform.rotation = Quaternion({0.0f,-1.0f,0.0f}).Normalize();
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
	// アニメーション用意
	runAnim[0].LoadAnimation("Player/C_Body_Run.gltf", &meshes[0]);
	runAnim[1].LoadAnimation("Player/C_Head_Run.gltf", &meshes[1]);
	runAnim[2].LoadAnimation("Player/C_LHand_Run.gltf", &meshes[2]);
	runAnim[3].LoadAnimation("Player/C_RHand_Run.gltf", &meshes[3]);

	meshes[1].transform.Parent(&meshes[0].transform);
	meshes[2].transform.Parent(&meshes[0].transform);
	meshes[3].transform.Parent(&meshes[0].transform);

	runAnim[0].Start();
	runAnim[1].Start();
	runAnim[2].Start();
	runAnim[3].Start();

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


	// Runアニメーション
	for (int i = 0; i < 4; i++) {
		meshes[i].material.enableLighting = true;
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


	if (dir.Length() > 0.0f) {
		// 目的の角度
		Vector3 goalEuler = { 0.0f, 0.0f, 0.0f };
		// Y軸周りの角度
		goalEuler.y = std::atan2f(dir.x, dir.z);
		// X軸周りの角度
		goalEuler.x = std::atan2f(-dir.y, Vector3{ dir.x, 0.0f, dir.z }.Length());

		// 現在の角度と目標の角度を比較し、逆回転の場合に調整
		if (std::abs(euler.y - goalEuler.y) > M_PI) {
			if (euler.y > goalEuler.y) {
				euler.y -= static_cast<float>(2.0f * M_PI);
			}
			else {
				euler.y += static_cast<float>(2.0f * M_PI);
			}
		}

		// 角度を適応
		meshes[0].transform.rotation = Quaternion::ConvertEuler(Utility::Interp::Slerp(euler, goalEuler, 0.2f));

	}

	ImGui::Begin("Test");
	Vector3 f = Vector3{ 1.0f,0.0f,0.0f } * Matrix4x4::CreateRotateXYZMatrix(meshes[0].transform.rotation);
	ImGui::DragFloat3("f", &f.x);
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
