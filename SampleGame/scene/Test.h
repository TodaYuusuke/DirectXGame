#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

#include "../class/particle/DamageParticle.h"

class Test final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Resource::RigidModel mesh;
	//LWP::Resource::Animation anim;
	LWP::Primitive::Sprite sprite;
	LWP::Object::Camera subCamera;

	LWP::Resource::RigidModel cube;
	LWP::Resource::RigidModel cube2;

	LWP::Object::PointLight pl;

	DamageParticle damageParticle_;
};