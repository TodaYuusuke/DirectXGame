#include "Title.h"
#include "NullScene.h"
#include "GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;
using namespace LWP::Info;

// 初期化
void Title::Initialize() {
	// 地形初期化
	field_.Init(&levelData, &mainCamera);

	ui_.material.texture = LWP::Resource::LoadTexture("ui/title.png");
	ui_.worldTF.translation = { 1176.0f, 800.0f };
	ui_.material.enableLighting = false;
	ui_.isUI = true;

	drone_.LoadShortPath("Drone/Drone.gltf");
	drone_.worldTF.translation = { -385.6f, 177.48f, -398.39f };
	drone_.worldTF.rotation = { 0.0f, 0.435f, 0.0f, 0.901f };
	drone_.worldTF.scale = { 1.0f,1.0f,1.0f };
	droneAnim_.LoadFullPath("resources/model/Drone/Drone.gltf", &drone_);
	droneAnim_.Play("00_Idle", true);
	
	// カメラ位置調整
	mainCamera.transform.translation = { -397.8f, 185.74f, -431.1f };
	mainCamera.transform.rotation = { 0.083f, 0.232f, -0.02f, 0.969f };

	// 演出初期化
	stagingSprite_.isUI = true;
	stagingSprite_.material.enableLighting = false;
	stagingSprite_.material.color = ColorPattern::BLACK;
	stagingSprite_.worldTF.scale = { 10.0f,5.5f,0.0f };
	// カメラの位置保存
	endStaging_.startPos = mainCamera.transform.GetWorldPosition();
	endStaging_.startRotation = mainCamera.transform.rotation;
}

// 更新
void Title::Update() {
	// 開始演出更新
	if (!startStaging_.end) {
		StartStaging& s = startStaging_;
		// 時間更新
		s.time += GetDeltaTimeF();
		// 最大時間を超過しないように
		if (s.time > s.kTime) {
			s.time = s.kTime;
			s.end = true;	// 演出終了
		}

		// tを計算
		float t = Easing::Liner(s.time / s.kTime);

		// フェードイン
		stagingSprite_.material.color.A = static_cast<unsigned char>(255 - t * 255);
	}
	// 通常更新処理
	else if (!endStaging_.start) {
		// SPACEキーを押すとシーン切り替え
		if (Keyboard::GetTrigger(DIK_P) || Controller::GetTrigger(XBOX_A)) {
			// 演出開始
			endStaging_.start = true;
		}
	}
	// 終了演出更新
	else {
		EndStaging& e = endStaging_;
		// 演出終了したらシーン遷移
		if (e.end) {
			nextSceneFunction = []() { return new NullScene([]() { return new GameScene(); }); };
		}
		else {
			// 時間更新
			e.time += GetDeltaTimeF();
			// 最大時間を超過しないように
			if (e.time > e.kTime) {
				e.time = e.kTime;
				e.end = true;	// シーン終了
			}

			// tを計算
			float t = Easing::InCubic(e.time / e.kTime);

			// フェードアウト
			stagingSprite_.material.color.A = static_cast<unsigned char>(t * 255);
			// ドローンの位置へカメラをアニメーション
			mainCamera.transform.translation = Interp::Lerp(e.startPos, drone_.worldTF.GetWorldPosition(), t);
			mainCamera.transform.rotation = Interp::SlerpQuaternion(e.startRotation, drone_.worldTF.rotation, t / 2.0f);
		}
	}
}