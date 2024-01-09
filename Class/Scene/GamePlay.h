#pragma once
#include "IScene.h"
#include "DebugCamera.h"
#include "MyClass/GameObject/FPSCamera/FPSCamera.h"
#include "../MyClass/GameObject/Field/Skydome/Skydome.h"
#include "../MyClass/GameObject/Field/Ground/Ground.h"

#include "../MyClass/GameObject/Character/CharacterManager.h"
#include <memory>

class GamePlay final : public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 3Dモデルデータ
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	std::unique_ptr<Model> groundModel_ = nullptr;

	// 一人称カメラ
	std::unique_ptr<FPSCamera> fpsCamera_ = nullptr;

	// スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// グラウンド
	std::unique_ptr<Ground> ground_ = nullptr;

	// 自キャラ
	std::unique_ptr<CharacterManager> characterManager_ = nullptr;
};