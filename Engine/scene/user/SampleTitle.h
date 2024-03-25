#pragma once
#include "../IScene.h"

class SampleTitle final
	: public IScene {
public:
	SampleTitle() = default;
	~SampleTitle() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Primitive::Triangle tri[2];
	LWP::Primitive::Surface surface;
	LWP::Primitive::Sphere sphere;
	LWP::Primitive::Cube cube;
	LWP::Primitive::Capsule capsule;
	LWP::Primitive::Sprite sprite[2];

	LWP::Primitive::Mesh ground;
	LWP::Primitive::Mesh wall[2];
	//LWP::Primitive::Mesh stressTestModel;	// 描画負荷検証用モデル

	LWP::Resource::Texture* uvTexture;
	LWP::Resource::Texture* monsterBall;

	// SEデータ
	LWP::Resource::Audio* audio;

	// サブカメラ
	LWP::Object::Camera subCamera[2];
	// 点光源
	LWP::Object::PointLight pl;

	// パーティクル
	LWP::Object::Particle* particle;

	// コライダー
	LWP::Object::Collider::AABB aabbCol;

	bool useMonsterBall = true;
};