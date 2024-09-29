#pragma once
#include "../class/field/Field.h"
#include "../class/player/Drone.h"
#include "scene/IScene.h" 

class GameScene final
	: public IScene {
public:
	
	// コンストラクタ
	GameScene(LWP::Math::Vector3 startPos, LWP::Math::Quaternion startRot);

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// 地形データ
	Field field_;

	// ドローン
	Drone drone_;

	// 開始時の地点
	LWP::Math::Vector3 startPos_;
	LWP::Math::Quaternion startRot_;


	// 画面遷移用UI
	LWP::Primitive::Sprite stagingSprite_;
	// シーン開始演出用
	struct StartStaging {
		// 経過時間
		float time = 0.0f;
		// アニメーションにかかる時間
		float kTime = 1.5f;

		// 終了フラグ
		bool end = false;
	}startStaging_;
	// シーン終了演出用
	struct EndStaging {
		// 経過時間
		float time = 0.0f;
		// アニメーションにかかる時間
		float kTime = 1.5f;

		// 開始フラグ
		bool start = false;
		// 終了フラグ
		bool end = false;
	}endStaging_;
};