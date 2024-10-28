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
	//ui_.material.texture = LWP::Resource::LoadTexture("ui/gameScene.png");
	//ui_.worldTF.translation = { 14.0f, 422.0f };
	//ui_.worldTF.scale = { 0.7f, 0.7f };
	//ui_.material.enableLighting = false;
	//ui_.isUI = true;

	// 地形初期化
	field_.Init(&levelData, &mainCamera);
	// カメラ管理に登録
	cameraManager_.Init(&levelData, &mainCamera, &statePattern_);
	// プレイヤー初期化
	player_.Init(&mainCamera);
	// 敵管理クラス初期化
	enemyManager_.Init(&levelData, &player_);

	// 演出初期化
	stagingSprite_.isUI = true;
	stagingSprite_.material.enableLighting = false;
	stagingSprite_.material.color = ColorPattern::BLACK;
	stagingSprite_.worldTF.scale = { 10.0f,5.5f,0.0f };

	// 状態管理クラスにフェードインアウトの処理を渡す
#pragma region FadeIn
	statePattern_.initFunction[int(BehaviorGS::FadeIn)] = [this](const BehaviorGS& pre) {
		pre;
		startStaging_.time = 0.0f;	// 時間初期化
		};
	statePattern_.updateFunction[int(BehaviorGS::FadeIn)] = [this](std::optional<BehaviorGS>& req, const BehaviorGS& pre) {
		StartStaging& s = startStaging_;
		// 時間更新
		s.time += GetDeltaTimeF();
		// 最大時間を超過しないように
		if (s.time > s.kTime) {
			s.time = s.kTime;
			req = BehaviorGS::Movie0;	// 演出終了
		}

		// tを計算
		float t = Easing::Liner(s.time / s.kTime);
		// フェードイン
		stagingSprite_.material.color.A = static_cast<unsigned char>(255 - t * 255);
		};
#pragma endregion
#pragma region FadeOut
	statePattern_.initFunction[int(BehaviorGS::FadeOut)] = [this](const BehaviorGS& pre) {
		pre;
		endStaging_.time = 0.0f;	// 時間初期化
		};
	statePattern_.updateFunction[int(BehaviorGS::FadeOut)] = [this](std::optional<BehaviorGS>& req, const BehaviorGS& pre) {
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


	statePattern_.initFunction[int(BehaviorGS::Play0)] = [this](const BehaviorGS& pre) {
		enemyManager_.StartWave(int(BehaviorGS::Play0) / 2 - 1);
	};
	statePattern_.updateFunction[int(BehaviorGS::Play0)] = [this](std::optional<BehaviorGS>& req, const BehaviorGS& pre) {
		if (enemyManager_.GetClearFlag()) { req = BehaviorGS::FadeOut; }	// 敵を倒したらフェードアウト
	};
	statePattern_.initFunction[int(BehaviorGS::Play1)] = [this](const BehaviorGS& pre) {
		enemyManager_.StartWave(int(BehaviorGS::Play1) / 2 - 1);
	};

	// 名前を登録しておく
	statePattern_.name[int(BehaviorGS::FadeIn)] = "FadeIn";
	statePattern_.name[int(BehaviorGS::Movie0)] = "Movie0";
	statePattern_.name[int(BehaviorGS::Play0)] = "Play0";
	statePattern_.name[int(BehaviorGS::Movie1)] = "Movie1";
	statePattern_.name[int(BehaviorGS::Play1)] = "Play1";
	statePattern_.name[int(BehaviorGS::FadeOut)] = "FadeOut";
}
// 更新
void GameScene::Update() {
	statePattern_.Update();
	// プレイ中の処理
	if (int(statePattern_.GetCurrentBehavior()) % 2 == 0) {
		player_.Update();
	}
	cameraManager_.Update();
	enemyManager_.Update();

#if DEMO
	ImGui::Begin("State");
	statePattern_.DebugGUI();
	ImGui::End();
	enemyManager_.DebugGUI();
#endif

	// シーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		levelData.HotReload();
	}
	// Pキーを押すとシーン切り替え
	if (Keyboard::GetTrigger(DIK_P)) {
		statePattern_.request = BehaviorGS::FadeOut;
	}
}
