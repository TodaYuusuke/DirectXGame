#include "GameScene.h"
#include "NullScene.h"
#include "Title.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;
using namespace LWP::Info;

GameScene::GameScene(LWP::Math::Vector3 startPos, LWP::Math::Quaternion startRot) {
	startPos_ = startPos;
	startRot_ = startRot;
}

// 初期化
void GameScene::Initialize() {
	// 操作UI
	ui_.material.texture = LWP::Resource::LoadTexture("ui/gameScene.png");
	ui_.worldTF.translation = { 14.0f, 422.0f };
	ui_.worldTF.scale = { 0.7f, 0.7f };
	ui_.material.enableLighting = false;
	ui_.isUI = true;

	// 地形初期化
	field_.Init(&levelData, &mainCamera);

	// 演出初期化
	stagingSprite_.isUI = true;
	stagingSprite_.material.enableLighting = false;
	stagingSprite_.material.color = ColorPattern::BLACK;
	stagingSprite_.worldTF.scale = { 10.0f,5.5f,0.0f };

	// 状態管理クラスにフェードインアウトの処理を渡す
#pragma region FadeIn
	statePattern_.initFunction[FadeIn] = [this](const Behavior& pre) {
		pre;
		startStaging_.time = 0.0f;	// 時間初期化
		};
	statePattern_.updateFunction[FadeIn] = [this](std::optional<Behavior>& req, const Behavior& pre) {
		StartStaging& s = startStaging_;
		// 時間更新
		s.time += GetDeltaTimeF();
		// 最大時間を超過しないように
		if (s.time > s.kTime) {
			s.time = s.kTime;
			req = Movie0;	// 演出終了
		}

		// tを計算
		float t = Easing::Liner(s.time / s.kTime);
		// フェードイン
		stagingSprite_.material.color.A = static_cast<unsigned char>(255 - t * 255);
		};
#pragma endregion
#pragma region FadeOut
	statePattern_.initFunction[FadeOut] = [this](const Behavior& pre) {
		pre;
		endStaging_.time = 0.0f;	// 時間初期化
		};
	statePattern_.updateFunction[FadeOut] = [this](std::optional<Behavior>& req, const Behavior& pre) {
		EndStaging& e = endStaging_;
		// 時間更新
		e.time += GetDeltaTimeF();
		// 最大時間を超過しないように
		if (e.time > e.kTime) {
			e.time = e.kTime;
			nextSceneFunction = []() { return new NullScene([]() { return new Title(); }); };	// シーン終了
		}

		// tを計算
		float t = Easing::InCubic(e.time / e.kTime);
		// フェードアウト
		stagingSprite_.material.color.A = static_cast<unsigned char>(t * 255);
		};
#pragma endregion

	// いったんMovie0
	statePattern_.updateFunction[Movie0] = [this](std::optional<Behavior>& req, const Behavior& pre) {
		// シーン再読み込み
		if (Input::Keyboard::GetTrigger(DIK_R)) {
			levelData.HotReload();
		}
		// Pキーを押すとシーン切り替え
		if (Keyboard::GetTrigger(DIK_P)) {
			req = FadeOut;
		}
	};
}
// 更新
void GameScene::Update() {
	statePattern_.Update();

	ImGui::Begin("Test");
	statePattern_.DebugGUI();
	ImGui::End();
}
