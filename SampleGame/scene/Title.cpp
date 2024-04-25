#include "Title.h"
#include "GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void Title::Initialize() {
	// デバッグ情報表示
	Info::ChangeShowDebugGUI();

	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTextureLongPath("resources/system/texture/uvChecker.png");
	monsterBall = LWP::Resource::LoadTextureLongPath("resources/system/texture/monsterBall.png");


	// サブカメラを初期化
	subCamera[0].transform.translation = { 5.9f,4.5f,-10.0f };
	subCamera[0].transform.rotation = { 0.4f,-0.5f,0.0f };
	// ポストプロセステスト
	subCamera[0].isUsePostProcess = true;
	subCamera[0].isActive = true;

	// サブカメラ2を初期化
	subCamera[1].shaderPath = "postProcess/SSAO.PS.hlsl";
	subCamera[1].transform.translation = { -5.9f,4.5f,-10.0f };
	subCamera[1].transform.rotation = { 0.4f,0.5f,0.0f };
	subCamera[1].isUsePostProcess = true;
	subCamera[1].isActive = false;
	

	// 三角形
	//for (int i = 0; i < 2; i++) {
	//	tri[i] = LWP::Common::CreateInstance<Triangle>();
	//}
	tri[0].vertices[0].color = Color(RED);
	tri[0].vertices[1].color = Color(BLUE);
	tri[0].vertices[2].color = Color(GREEN);
	tri[0].name = "Triangle0";
	tri[1].transform.rotation.y = 1.0f;
	tri[1].texture = uvTexture;
	tri[1].name = "Triangle1";

	// 平面
	//surface = LWP::Common::CreateInstance<Surface>();
	surface.transform.translation.x = -0.7f;
	surface.texture = uvTexture;
	surface.isActive = false;
	surface.name = "Surface";

	// 球
	//sphere = LWP::Common::CreateInstance<Sphere>();
	sphere.radius = 0.3f;
	sphere.transform.translation.x = -1.0f;
	sphere.transform.translation.z = -1.0f;
	sphere.material.enableLighting = true;
	sphere.material.shininess = 40.0f;
	sphere.texture = uvTexture;
	sphere.name = "Sphere";

	// 立方体
	cube.texture = monsterBall;
	cube.material.enableLighting = true;
	cube.name = "Cube";

	// カプセル
	capsule.transform.translation = { 1.0f,-0.2f,-2.0f };
	capsule.end = { 2.4f,1.2f,1.7f };
	capsule.radius = 0.2f;
	capsule.material.enableLighting = true;
	capsule.name = "Capsule";


	// 地面モデル
	ground.LoadFile("cube/cube.obj");
	ground.transform.translation.y = -1.5f;
	ground.transform.scale = { 10.0f,0.1f, 10.0f };
	ground.material.enableLighting = true;
	ground.name = "Ground";

	// 壁モデル
	wall[0].LoadFile("cube/cube.obj");
	wall[0].transform.translation = { 0.0f,-5.0f,3.0f };
	wall[0].transform.scale = { 10.0f,10.0f, 0.05f };
	wall[0].material.enableLighting = true;
	wall[0].isActive = true;
	wall[0].name = "Wall0";
	// 左側に壁を置く
	wall[1].LoadFile("cube/cube.obj");
	wall[1].transform.translation = {-2.1f, -3.0f, -2.0f};
	wall[1].transform.rotation.y = -1.54f;
	wall[1].transform.scale = { 5.0f,5.0f, 0.05f };
	wall[1].material.enableLighting = true;
	wall[1].isActive = true;
	wall[1].name = "Wall1";
	// テストモデル
	//stressTestModel = LWP::Resource::LoadModel("RGM-96XJesta_13_td.obj");
	//stressTestModel->material.enableLighting = true;
	//stressTestModel->isActive = true;


	// 平行光源
	dl.isActive = true;
	dl.intensity = 0.3f;

	// 点光源
	pl.transform.translation = { -0.94f,0.0f,-2.7f };
	pl.radius = 13.0f;
	pl.intensity = 0.3f;
	pl.isActive = true;

	// 複数画面描画の結果を張り付けるスプライト
	sprite[0].texture = subCamera[0].GetTexture();
	sprite[0].isUI = true;
	sprite[0].isActive = false;
	sprite[0].name = "Sprite0";
	sprite[1].transform.translation.x = 640.0f;
	sprite[1].texture = subCamera[1].GetTexture();
	sprite[1].isUI = true;
	sprite[1].isActive = false;
	sprite[1].name = "Sprite1";


	// パーティクルテスト
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
	particle.isActive = true;



	//aabbCol.CreateFromPrimitive(&cube);
	//// 試しにラムダ式を入れてみる
	//aabbCol.SetOnCollisionLambda([](Collider::HitData data) {
	//	if (data.state == Collider::OnCollisionState::None) {
	//		Utility::Log("None\n");
	//	}
	//	else {
	//		Utility::Log("Not None\n");
	//	}
	//});
		
	//Collider::AABB* aabbCol2 = LWP::Common::CreateInstance<Collider::AABB>();
	//aabbCol2->Create({ 2.0f,0.0f,0.0f });
	//Collider::AABB* aabbCol3 = LWP::Common::CreateInstance<Collider::AABB>();
	//aabbCol3->Create({ -2.0f,0.0f,0.0f });

	//Collider::Sphere* sphereCol0 = LWP::Common::CreateInstance<Collider::Sphere>();
	//sphereCol0->Create({ 3.0f,0.0f,0.0f });

	//// カプセル実験
	//Collider::Capsule* capsule = LWP::Common::CreateInstance<Collider::Capsule>();
	//capsule->Create({ 0.0f,0.0f,-3.0f }, { 0.0f,1.0f,-3.0f }, 0.5f);
}

// 更新
void Title::Update() {
	// ポストプロセスの切り替え
	if (Keyboard::GetTrigger(DIK_SPACE)) {
		mainCamera.isUsePostProcess = !mainCamera.isUsePostProcess;
	}

	// Tキーを押すとテクスチャ切り替え
	if (Keyboard::GetTrigger(DIK_T)) {
		//if (!useMonsterBall) {
		//	sphere->texture = monsterBall;
		//	useMonsterBall = true;
		//}
		//else {
		//	sphere->texture = uvTexture;
		//	useMonsterBall = false;
		//}
	}

	if (Keyboard::GetTrigger(DIK_1)) {
		LWP::Window::ChangeWindowMode();
	}
	else if (Keyboard::GetTrigger(DIK_2)) {
		LWP::Window::ChangeFullScreenMode();
	}
	else if (Keyboard::GetTrigger(DIK_3)) {
		LWP::Window::ChangeBorderlessWindowMode();
	}

	// シェーダー作り直し
	if (Keyboard::GetTrigger(DIK_R)) {
		//mainCamera.ReCreateShader();
	}
	// パーティクル呼び出し
	if (Keyboard::GetTrigger(DIK_P)) {
		particle.Add(16);
	}
	// プログラム終了
	if (Keyboard::GetTrigger(DIK_O)) {
		LWP::System::End();
	}

	// ESCキーでデバッグ情報表示切り替え
	if (Keyboard::GetTrigger(DIK_ESCAPE)) {
		Info::ChangeShowDebugGUI();
	}

	// ENTERキーを押すとシーン切り替え
	if (Keyboard::GetTrigger(DIK_N)) {
		nextSceneFunction = []() { return new GameScene(); };
	}
}