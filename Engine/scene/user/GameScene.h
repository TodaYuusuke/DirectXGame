#pragma once
#include "../IScene.h" 
#include "../../../Class/MapChip.h"

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

	LWP::Primitive::Triangle* tri;

	//マップチップ
	MapChip* mapChip_;
	//マップ位置
	LWP::Math::Vector3 mapPosition_;
};