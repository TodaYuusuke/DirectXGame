#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

class LevelLoaderTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	//LWP::Primitive::Triangle tri_[2];
	//LWP::Primitive::Surface sur_[2];

	//LWP::Primitive::Surface box_[3];

	LWP::Object::PointLight light_;
};