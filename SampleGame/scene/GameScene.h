#pragma once
#include "../class/field/Field.h"
#include "../class/player/Player.h"
#include "../class/player/Drone.h"
#include "../class/car/Car.h"
#include "scene/IScene.h" 

class GameScene final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// 地形データ
	Field field_;

	// プレイヤー
	Player player_;
	// カメラ
	Car car_;
	// ドローン
	Drone drone_;
};