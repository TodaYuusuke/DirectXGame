#include "SampleGameScene.h"
#include "SampleTitle.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void SampleGameScene::Initialize() {
	// 球
	//sphere = new Sphere();
	sphere.radius = 0.3f;
	sphere.transform.translation.x = 0.0f;
	sphere.transform.translation.z = 3.0f;
	sphere.material.enableLighting = true;
	sphere.material.shininess = 40.0f;

	motion.Add(&sphere.transform.translation, Vector3{ 1.0f,0.0f,0.0f }, 0.0f, 1.0f).
		Add(&sphere.transform.translation, Vector3{ 0.0f,1.0f,0.0f }, 1.0f, 1.0f).
		Add(&sphere.transform.translation, Vector3{ 0.0f,-1.0f,0.0f }, 2.0f, 1.0f);

	particle.SetPrimitive<Primitive::Billboard2D>();
	particle.initFunction = [](Primitive::IPrimitive* primitive) {
		Object::ParticleData newData{};
		newData.wtf.translation = primitive->transform.GetWorldPosition();
		newData.wtf.rotation = primitive->transform.rotation;
		newData.wtf.scale = primitive->transform.scale;

		// 速度ベクトルを生成
		int dir1 = Utility::GenerateRandamNum<int>(-100, 100);
		int dir2 = Utility::GenerateRandamNum<int>(-100, 100);
		int dir3 = Utility::GenerateRandamNum<int>(-100, 100);
		// 発射のベクトル
		Math::Vector3 dir{ dir1 / 100.0f,dir2 / 100.0f, dir3 / 100.0f };
		newData.velocity = dir.Normalize() * 0.2f;

		// パーティクル追加
		return newData;
		};
	particle.updateFunction = [](Object::ParticleData* data) {
		// 経過フレーム追加
		data->elapsedFrame++;

		data->wtf.translation += data->velocity;	// 速度ベクトルを加算
		data->wtf.rotation += data->velocity;	// ついでに回転させとく
		data->wtf.translation.y += -9.8f / 100.0f;	// 重力を加算

		// 速度ベクトルを弱める
		data->velocity *= 0.9f;

		return data->elapsedFrame > 180 ? true : false;
		};
	particle.P()->texture = Resource::LoadTexture("AlphaTest.png");
	particle.isActive = true;

	se.Load("powerup01.mp3");
}
// 更新
void SampleGameScene::Update() {
	// SPACEキーを押すとアニメーション
	if (Keyboard::GetTrigger(DIK_M)) {
		motion.Start();
	}

	if (Keyboard::GetTrigger(DIK_SPACE)) {
		se.Play(0.5f);
		particle.Add(16);
	}
	// Rキーを押すとシーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextSceneFunction = []() { return new SampleGameScene(); };
	}
	// ENTERキーを押すとシーン切り替え
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextSceneFunction = []() { return new SampleTitle(); };
	}
}