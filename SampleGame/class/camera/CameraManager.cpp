#include "CameraManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Info;

// 初期化
void CameraManager::Init(LevelData* level, Camera* camera,
	StatePattern<BehaviorGS, size_t(BehaviorGS::Count)>* state, int* wave) {
	level_ = level;
	camera_ = camera;
	state_ = state;
	wave_ = wave;

	// カメラの初期設定
	camera_->fov = 120.0f;
	camera_->worldTF.translation = level_->catmullRomCurves[GetRouteName()].GetPosition(0.0f);

	// 黒帯初期化
	blackBelt_.Init();

	state_->initFunction[int(BehaviorGS::Movie)] = [this](const BehaviorGS& pre) {
		route_ = &level_->catmullRomCurves[GetRouteName()];	// 使用する曲線を受け取る
		focus_ = &level_->catmullRomCurves[GetFocusName()];
		route_->t = 0.0f;	// 初期化
		blackBelt_.SetIsActive(true);
	};
	state_->updateFunction[int(BehaviorGS::Movie)] = [this](std::optional<BehaviorGS>& req, const BehaviorGS& pre) {
		// 曲線の処理を行う
		float& rT = route_->t;	// 現在のt
		float& fT = focus_->t;
		rT += GetDeltaTimeF() * 0.2f;	// デフォルトの速度と早すぎるので減速
		// tが1以上にならないように修正
		if (rT > 1.0f) {
			rT = 1.0f;
			// tが1以上ならシーン変更処理
			req = BehaviorGS::Play;	// シーンをプレイに移す
			blackBelt_.SetIsActive(false);
		}
		fT = rT;	// 同期

		// カメラの座標をセット
		camera_->worldTF.translation = route_->GetPosition();
		// 向く方向の座標をセット
		Vector3 viewDirectionPos = focus_->GetPosition();

		// カメラの方向をセット
		Vector3 from = Vector3{ 0.0f,0.0,1.0f } *camera_->worldTF.rotation;	// 現在向いている方向
		Vector3 to = viewDirectionPos - camera_->worldTF.translation;	// 次に向く方向
		Quaternion q = camera_->worldTF.rotation * Quaternion::DirectionToDirection(from.Normalize(), to.Normalize());
		q.z = 0.0f;	// Z回転は無視
		camera_->worldTF.rotation = Interp::SlerpQuaternion(camera_->worldTF.rotation, q, 0.1f).Normalize();
	};
}

void CameraManager::Update() {
	blackBelt_.Update();

#if DEMO
	ImGui::Begin("CameraManager");
	if (route_ && ImGui::TreeNode("Route")) {
		route_->DebugGUI();
		ImGui::TreePop();
	}
	if (focus_ && ImGui::TreeNode("Focus")) {
		focus_->DebugGUI();
		ImGui::TreePop();
	}
	ImGui::End();
#endif
}

std::string CameraManager::GetRouteName() {
	return "Movie" + std::to_string(*wave_);
}
std::string CameraManager::GetFocusName() {
	return "Focus" + std::to_string(*wave_);
}

