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
	StatePattern<BehaviorGS, size_t(BehaviorGS::Count)>* state) {
	level_ = level;
	camera_ = camera;
	state_ = state;

	// カメラの初期設定
	camera_->fov = 120.0f;
	camera_->transform.translation = level_->catmullRomCurves["Movie0"].GetPosition(0.0f);

	// 黒帯初期化
	blackBelt_.Init();

	state_->initFunction[int(BehaviorGS::Movie0)] = [this](const BehaviorGS& pre) {
		catmullRom_ = &level_->catmullRomCurves["Movie0"];	// 使用するアニメーションを設定
		catmullRom_->t = 0.0f;	// 初期化
		blackBelt_.SetIsActive(true);
	};
	state_->initFunction[int(BehaviorGS::Movie1)] = [this](const BehaviorGS& pre) {
		catmullRom_ = &level_->catmullRomCurves["Movie1"];	// 使用するアニメーションを設定
		catmullRom_->t = 0.0f;	// 初期化
		blackBelt_.SetIsActive(true);
	};
}

void CameraManager::Update() {
	blackBelt_.Update();

	// 曲線の処理を行う
	if (catmullRom_) {
		float& t = catmullRom_->t;	// 現在のt
		t += GetDeltaTimeF() * 0.2f;
		float nextT = t + 0.1f;	// カメラの向く方向用のt

		// tが1以上にならないように修正
		if (t > 1.0f) { t = 1.0f; }
		if (nextT > 1.0f) { nextT = 1.0f; }

		// カメラの座標をセット
		camera_->transform.translation = catmullRom_->GetPosition();
		// 向く方向の座標をセット
		Vector3 viewDirectionPos = catmullRom_->GetPosition(nextT);
		if (nextT >= 1.0f) {	// もしnextTがcatmullRomの外側ならば、Z+方向を向く
			viewDirectionPos.z += 1.0f;
		}

		// カメラの方向をセット
		Vector3 from = Vector3{ 0.0f,0.0,1.0f } * camera_->transform.rotation;	// 現在向いている方向
		Vector3 to = viewDirectionPos - camera_->transform.translation;	// 次に向く方向
		Quaternion q = camera_->transform.rotation * Quaternion::DirectionToDirection(from.Normalize(), to.Normalize());
		q.z = 0.0f;	// Z回転は無視
		camera_->transform.rotation = Interp::SlerpQuaternion(camera_->transform.rotation, q, 0.1f).Normalize();

#if DEMO
		ImGui::Begin("CameraManager");
		catmullRom_->DebugGUI();
		ImGui::End();
#endif
		// tが1以上ならシーン変更処理
		if (catmullRom_->t >= 1.0f) {
			state_->request = BehaviorGS(int(state_->GetCurrentBehavior()) + 1);	// 次のシーンにする
			blackBelt_.SetIsActive(false);
			catmullRom_ = nullptr;
		}
	}
}
