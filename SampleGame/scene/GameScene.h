#pragma once
#include "scene/IScene.h" 

#include "class/camera/CameraManager.h"
#include "class/player/Player.h"
#include "class/enemy/EnemyManager.h"
#include "class/field/Field.h"
#include "class/Behavior.h"

class GameScene final
	: public IScene {
public: // ** 純粋仮想関数の実体宣言 ** //

	// 初期化
	void Initialize();
	// 更新
	void Update();


private: // ** これより先に必要な処理や変数を記述 ** //

	// 状態管理
	LWP::Utility::StatePattern<BehaviorGS, size_t(BehaviorGS::Count)> statePattern_;
	// 最大ウェーブ数
	const int kMaxWave_ = 2;
	// 現在のウェーブ
	int wave_ = 0;

	// 地形データ
	Field field_;
	// カメラ管理
	CameraManager cameraManager_;
	// プレイヤー
	Player player_;
	// 敵管理
	EnemyManager enemyManager_;

	// 操作表示
	LWP::Primitive::Sprite ui_;


	// 画面遷移用UI
	LWP::Primitive::Sprite stagingSprite_;
	// シーン開始演出用
	struct StartStaging {
		// 経過時間
		float time = 0.0f;
		// アニメーションにかかる時間
		float kTime = 1.5f;
	}startStaging_;
	// シーン終了演出用
	struct EndStaging {
		// 経過時間
		float time = 0.0f;
		// アニメーションにかかる時間
		float kTime = 1.5f;
	}endStaging_;
};