#pragma once
#include "../IScene.h" 
#include "../../../Class/Map/MapManager.h"
#include "../../../Class/Hammer/Hammer.h"

class GameScene final
	: public IScene {
public:

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// マップ管理クラス
	MapManager mapManager_;
	// ハンマー（プレイヤー）
	Hammer hammer_;
};