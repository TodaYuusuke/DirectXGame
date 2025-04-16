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

void GameScene::Initialize() {
	// 地形初期化
	field_.Init(&levelData, &mainCamera);
	// カメラ管理に登録
	cameraManager_.Init(&levelData, &mainCamera, &statePattern_, &wave_);
	// プレイヤー初期化
	player_.Init(&mainCamera);
	// 敵管理クラス初期化
	enemyManager_.Init(&levelData, &player_);

	// UI描画
	//ui_.material.texture = Resource::LoadTexture("ui/Control.png");
	ui_.anchorPoint = { 0.0f,0.5f };
	//ui_.worldTF.translation.y = 1080.0f / 2.0f;
	//ui_.worldTF.scale = { 0.8f,0.8f,1.0f };

	// 演出初期化
	//stagingSprite_.isUI = true;
	//stagingSprite_.material.enableLighting = false;
	//stagingSprite_.material.color = ColorPattern::BLACK;
	//stagingSprite_.worldTF.scale = { 10.0f,5.5f,0.0f };

	// 状態管理クラスにフェードインアウトの処理を渡す
#pragma region FadeIn
	statePattern_.initFunction[int(BehaviorGS::FadeIn)] = [this]([[maybe_unused]] const BehaviorGS& pre) {
		pre;
		startStaging_.time = 0.0f;	// 時間初期化
		};
	statePattern_.updateFunction[int(BehaviorGS::FadeIn)] = [this]([[maybe_unused]] std::optional<BehaviorGS>& req, [[maybe_unused]] const BehaviorGS& pre) {
		StartStaging& s = startStaging_;
		// 時間更新
		s.time += GetDeltaTimeF();
		// 最大時間を超過しないように
		if (s.time > s.kTime) {
			s.time = s.kTime;
			req = BehaviorGS::Movie;	// 演出終了
		}

		// tを計算
		float t = Easing::Liner(s.time / s.kTime);
		// フェードイン
		//stagingSprite_.material.color.A = static_cast<unsigned char>(255 - t * 255);
		};
#pragma endregion
#pragma region FadeOut
	statePattern_.initFunction[int(BehaviorGS::FadeOut)] = [this]([[maybe_unused]] const BehaviorGS& pre) {
		pre;
		endStaging_.time = 0.0f;	// 時間初期化
		};
	statePattern_.updateFunction[int(BehaviorGS::FadeOut)] = [this]([[maybe_unused]] std::optional<BehaviorGS>& req, [[maybe_unused]] const BehaviorGS& pre) {
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
		//stagingSprite_.material.color.A = static_cast<unsigned char>(t * 255);
	};
#pragma endregion


	statePattern_.initFunction[int(BehaviorGS::Play)] = [this]([[maybe_unused]] const BehaviorGS& pre) {
		enemyManager_.StartWave(wave_);	// ウェーブ開始
	};
	statePattern_.updateFunction[int(BehaviorGS::Play)] = [this]([[maybe_unused]] std::optional<BehaviorGS>& req, [[maybe_unused]] const BehaviorGS& pre) {
		if (enemyManager_.GetClearFlag()) {
			wave_++;	// 次のウェーブへ
			// ウェーブが最後までいったらフェードアウト
			if (wave_ >= kMaxWave_) { req = BehaviorGS::FadeOut; }
			// そうでないなら次のムービーへ
			else { req = BehaviorGS::Movie; }
		}
	};

	// 名前を登録しておく
	statePattern_.name[int(BehaviorGS::FadeIn)] = "FadeIn";
	statePattern_.name[int(BehaviorGS::Movie)] = "Movie";
	statePattern_.name[int(BehaviorGS::Play)] = "Play";
	statePattern_.name[int(BehaviorGS::FadeOut)] = "FadeOut";
}
// 更新
void GameScene::Update() {
	statePattern_.Update();
	// プレイ中の処理
	if (statePattern_.GetCurrentBehavior() == BehaviorGS::Play) {
		player_.Update();
	}
	cameraManager_.Update();
	enemyManager_.Update();

#if DEMO
	ImGui::Begin("State");
	ImGui::SliderInt("Wave", &wave_, 0, kMaxWave_ - 1);
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
