#pragma once
#include "scene/IScene.h"

class Result final : public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入
private: //*** これより先に必要な処理や変数を記述 ***//
	// 背景素材
	LWP::Primitive::Sprite* backGround_ = nullptr;
	// サウンドデータハンドル
	//uint32_t gameStartSE = {0};
};