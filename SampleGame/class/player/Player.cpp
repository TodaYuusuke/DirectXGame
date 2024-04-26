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
	//camera_->transform.rotation.x = 0.65f;
	//cameraGoalRotation_ = camera_->transform.rotation;
	camera_->transform.translation = meshes[0].transform.translation + (cameraOffset_ * Matrix4x4::CreateRotateXYZMatrix(cameraGoalRotation_));
	cameraGoalTranslation_ = camera_->transform.translation;

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
	// 移動
	Math::Vector2 dir = { 0.0f,0.0f };
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
	Matrix4x4 cameraRotationMatrix = Matrix4x4::CreateRotateXYZMatrix(Vector3{0.0f, camera_->transform.rotation.y, 0.0f});
	meshes[0].transform.translation += Math::Vector3{ dir.x, 0.0f, dir.y } * kPlayerSpeed * cameraRotationMatrix;
	// 向きを設定
	if (dir.Length() > 0.0f) {
		// 現在の方向ベクトル
		//Math::Vector3 cuurentDir;
		//cuurentDir = Vector3{ 1.0f,0.0f,0.0f } * Matrix4x4::CreateRotateXYZMatrix(meshes[0].transform.rotation);
		//Vector3 temp = meshes[0].
		//
		//meshes->transform.rotation = meshes->transform.rotation * Matrix4x4::DirectionToDirection(cuurentDir,{ dir.x, 0.0f, dir.y });

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
	//camera_->transform.rotation = Utility::Interp::Lerp(camera_->transform.rotation, cameraGoalRotation_, 0.06f);
	// 方向ベクトルを求める
	//Vector3 direction = camera_->transform.translation - meshes[0].transform.translation;
	//camera_->transform.rotation = Quaternion::ConvertDirectionVector(direction.Normalize());
}
