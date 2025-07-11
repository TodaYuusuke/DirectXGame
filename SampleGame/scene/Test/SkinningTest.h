#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

class SkinningTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Resource::SkinningModel model[2];
	LWP::Resource::Animation anim[2];

	LWP::Object::PointLight pl;
	LWP::Object::DirectionLight dl;
	LWP::Primitive::NormalSprite sp;
};