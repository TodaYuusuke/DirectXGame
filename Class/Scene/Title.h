#pragma once
#include "scene/IScene.h"

class Title final : public IScene {
public:
	Title() = default;
	~Title() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//;
	// 背景素材
	LWP::Primitive::Sprite* titleSprite_ = nullptr;
	// サウンドデータハンドル
	//LWP::Resource::Audio* gameStartSE_ = nullptr;
};