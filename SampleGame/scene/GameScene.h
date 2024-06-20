#pragma once
#include "../class/player/Player.h"
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

	// ビル群
	LWP::Resource::RigidModel buildings[8];
	// スカイドーム
	LWP::Resource::RigidModel skydome;
	// ☆
	const static int kStarCount = 512;
	LWP::Resource::RigidModel stars[kStarCount];
	// 地面
	LWP::Resource::StaticModel ground;


	// プレイヤー
	Player player;
};