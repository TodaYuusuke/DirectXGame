#pragma once
#include <Adapter.h>
#include "scene/IScene.h" 

class GameScene final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//
	LWP::Primitive::Sphere sphere;
	LWP::Resource::Motion motion;
	LWP::Object::Particle particle;
	LWP::Resource::Audio se;

	//LWP::Primitive::Triangle tri;
	//LWP::Primitive::Surface surface;
	//LWP::Primitive::Cube cube;
	//LWP::Primitive::Capsule capsule;
	//LWP::Primitive::Sprite sprite[2];

	//LWP::Primitive::Mesh ground;
	//LWP::Primitive::Mesh wall[2];

	//LWP::Resource::Texture uvTexture;
	//LWP::Resource::Texture monsterBall;

	// サブカメラ
	//LWP::Object::Camera subCamera[2];
	// 平行光源
	//LWP::Object::DirectionLight dl;	// 平気だった
	// 点光源
	//LWP::Object::PointLight pl;	// 平気だった

	// コライダー
	LWP::Object::Collider::AABB aabbCol;
};