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

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// 地面
	LWP::Primitive::Surface* ground;

	LWP::Primitive::Triangle* tri[2];
	LWP::Primitive::Surface* surface;
	LWP::Primitive::Sphere* sphere;
	
	LWP::Primitive::Mesh* cubeModel;
	LWP::Primitive::Mesh* stressTestModel;	// 描画負荷検証用モデル

	LWP::Resource::Texture* uvTexture;
	LWP::Resource::Texture* monsterBall;

	// SEデータ
	LWP::Resource::Audio* audio;

	bool useMonsterBall = true;
};