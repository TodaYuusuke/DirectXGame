#include "Drone.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Utility;

// 初期化
void Drone::Init(LWP::Object::Camera* ptr) {
	// モデル用意
	model_.LoadShortPath("Drone/Drone.gltf");
	model_.SetAllMaterialLighting(true);
	model_.isActive = false;
	// コライダー
	collision_.SetFollowTarget(&model_.worldTF);
	collision_.name = "Drone";
	collision_.isMove = true;
	collision_.mask.SetBelongFrag(lwpC::Collider::Player);	// フラグ設定
	collision_.mask.SetHitFrag(lwpC::Collider::FieldObject | lwpC::Collider::Terrain);
	
	// アニメーション用意
	anim_.LoadFullPath("resources/model/Drone/Drone.gltf", &model_);

	// 点光源
	pointLight_.transform.Parent(&model_.worldTF);
	pointLight_.transform.translation.z = 2.0f;
	pointLight_.transform.translation.y = 1.75f;
	pointLight_.radius = 13.0f;
	pointLight_.intensity = 0.7f;
	pointLight_.isActive = false;

	particle_.model.LoadCube();
	particle_.model.worldTF.scale *= 0.4f;

	// カメラのポインタをセット
	cameraPtr_ = ptr;
	cameraPtr_->transform.Init();	// 座標系を初期化
	cameraPtr_->transform.Parent(&model_.worldTF);
}

// 更新
void Drone::Update() {
	// 各種更新処理
	Move();
	CameraMove();
	Shot();

	// 速度を加算
	model_.worldTF.translation += velocity_;

	// 速度を減衰させる
	velocity_ *= kDecayRate_;
	// 前フレームで地形にヒットしていたなら重力加速度をリセット

#if DEMO
	ImGui::Begin("Drone");
	if (ImGui::TreeNode("Model")) {
		model_.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::DragFloat("kSpeed", &kSpeed_, 0.01f);
	ImGui::DragFloat("kCameraSpeed", &kCameraSpeed_, 0.01f);
	ImGui::DragFloat("kDecayRate", &kDecayRate_, 0.01f);
	ImGui::End();
#endif
}

void Drone::SetModelTF(LWP::Math::Vector3 pos, LWP::Math::Quaternion rotation) {
	model_.worldTF.translation = pos;
	model_.worldTF.rotation = rotation;
}

void Drone::Move() {
	// 移動する向き
	Vector3 dir = { 0.0f,0.0f,0.0f };

	// LTが押されている間は上下移動操作
	if (Controller::GetPress(XBOX_LT)) {
		// コントローラーでの移動
		dir.y += Pad::GetLStick().y;
	}
	else {
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

		dir = Vector3(dir * Matrix4x4::CreateRotateXYZMatrix(model_.worldTF.rotation)).Normalize();
		dir = Vector3{ dir.x, 0.0f, dir.z };

		// 上下移動
		if (Keyboard::GetPress(DIK_SPACE)) {
			dir.y = 1.0f;
		}
		if (Keyboard::GetPress(DIK_C)) {
			dir.y = -1.0f;
		}
	}

	// 正規化
	dir = dir.Normalize();

	// シフトを押しているならダッシュ
	if (Keyboard::GetPress(DIK_LSHIFT)) {
		dir *= 1.9f;
	}

	// 速度を加算
	velocity_ += dir * kSpeed_ * Info::GetDeltaTimeF();
}

void Drone::CameraMove() {
	// 移動する向き
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

	// コントローラーでの移動
	dir.x -= Pad::GetRStick().y;
	dir.y += Pad::GetRStick().x;
	
	// 正規化して速度調整
	dir = dir.Normalize();
	dir *= kCameraSpeed_;

	model_.worldTF.rotation =
		Quaternion::CreateFromAxisAngle(Vector3::UnitY(), dir.y) * model_.worldTF.rotation * Quaternion::CreateFromAxisAngle(Vector3::UnitX(), dir.x);
}

void Drone::Shot() {
	// 弾を更新
	for (Bullet& b : bullets_) {
		b.Update();
	}
	// 死亡した弾を削除
	bullets_.remove_if([](Bullet& data) {
		if (!data.GetAlive()) {
			return true;
		}
		return false;
	});

	// SpaceかRTを押したら射撃
	if (Keyboard::GetTrigger(DIK_SPACE) || Controller::GetTrigger(XBOX_RT)) {
		// 弾を7発生成
		for (int i = 0; i < 1; i++) {
			// 方向ベクトルを計算
			Vector3 dir = Vector3::UnitZ() * (
				Quaternion::CreateFromAxisAngle(Vector3::UnitY(), float(GenerateRandamNum<int>(-20, 20)) / 1000.0f) *
				model_.worldTF.rotation *
				Quaternion::CreateFromAxisAngle(Vector3::UnitX(), float(GenerateRandamNum<int>(-20, 20)) / 1000.0f));
			bullets_.emplace_back(
				model_.worldTF.GetWorldPosition(),
				dir,
				terrainPtr_,
				&particle_
			);
		}
	}
}