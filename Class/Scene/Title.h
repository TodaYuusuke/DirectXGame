#pragma once
#include "IScene.h"
#include <Sprite.h>

class Title final : public IScene {
public:
	Title() = default;
	~Title() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//;
	bool preX = true; // XTrigger検知用

	// 背景素材
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	// サウンドデータハンドル
	uint32_t gameStartSE = {0};
};