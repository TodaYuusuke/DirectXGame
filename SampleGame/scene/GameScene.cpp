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

	// プレイヤー初期化
	drone_.Init(&mainCamera, levelData.terrain.get());
	drone_.SetModelTF(startPos_, startRot_);

	// 演出初期化
	stagingSprite_.isUI = true;
	stagingSprite_.material.enableLighting = false;
	stagingSprite_.material.color = ColorPattern::BLACK;
	stagingSprite_.worldTF.scale = { 10.0f,5.5f,0.0f };
}
// 更新
void GameScene::Update() {
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
		stagingSprite_.material.color.A = 255 - t * 255;
	}
	// 通常更新処理
	else if (!endStaging_.start) {
		drone_.Update();

		// シーン再読み込み
		if (Input::Keyboard::GetTrigger(DIK_R)) {
			levelData.HotReload();
		}
		// Pキーを押すとシーン切り替え
		if (Keyboard::GetTrigger(DIK_P)) {
			endStaging_.start = true;
		}
	}
	// 終了演出更新
	else {
		EndStaging& e = endStaging_;
		// 演出終了したらシーン遷移
		if (e.end) {
			nextSceneFunction = []() { return new NullScene([]() { return new Title(); }); };
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
			stagingSprite_.material.color.A = t * 255;
		}
	}
}