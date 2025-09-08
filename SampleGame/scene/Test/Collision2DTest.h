#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

class Collision2DTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	// 当たり判定
	LWP::Object::Collision2D circle[2];
	// 当たり判定
	LWP::Object::Collision2D rectangle[2];

	LWP::Primitive::NormalSprite sp_;
};