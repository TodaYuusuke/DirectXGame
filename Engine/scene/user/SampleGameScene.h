#pragma once
#include "../IScene.h" 
#include "../../../Class/Map/MapManager.h"
#include "../../../Class/Hammer/Hammer.h"
#include "../../../Class/Mob/MobManager.h"
#include "../Class/Skydome/Skydome.h"
#include "../Class/Tambourine/Tambourine.h"

class SampleGameScene final
	: public IScene {
public:

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// マップ管理クラス
	MapManager mapManager_;
	// ハンマー（プレイヤー）
	Hammer hammer_;
	// スカイドーム
	Skydome skydome_;
	// タンバリン
	Tambourine tambourine_;

	// モブ管理クラス
	MobManager mobManager_;
};