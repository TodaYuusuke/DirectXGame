#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

class AnimationTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	AnimationTest();

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Object::DirectionLight light;
	LWP::Object::PointLight p;

	LWP::Resource::RigidModel cube;
	LWP::Resource::SkinningModel model[5];
	LWP::Resource::SkinningModel weapon;
	LWP::Resource::Animation anim[5];
	//LWP::Resource::Motion motion[5];

	// 通常のスプライト
	LWP::Primitive::NormalSprite normalSprite;
	// 連番テクスチャ用のスプライト
	LWP::Primitive::SequenceSprite sequenceSprite;
	// 切り抜き描画用のスプライト
	LWP::Primitive::ClipSprite clipSprite;

	LWP::Object::Collision col;
	LWP::Object::Collider::Capsule& caps;
};