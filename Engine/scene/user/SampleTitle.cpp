#include "SampleTitle.h"
#include "SampleGameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void SampleTitle::Initialize() {
	// デバッグ情報表示
	Info::ChangeShowDebugGUI();

	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTextureLongPath("resources/system/texture/uvChecker.png");
	monsterBall = LWP::Resource::LoadTextureLongPath("resources/system/texture/monsterBall.png");

	// サブカメラを生成
	subCamera = LWP::Common::CreateInstance<LWP::Object::Camera>();
	LWP::Object::CreateRenderTexture(subCamera, 640, 360);
	subCamera->transform.translation = { 5.9f,4.5f,-10.0f };
	subCamera->transform.rotation = { 0.4f,-0.5f,0.0f };
	// ポストプロセステスト
	subCamera->isUsePostProcess = true;
	subCamera->isActive = true;

	// サブカメラを生成
	LWP::Object::Camera* c = LWP::Common::CreateInstance<LWP::Object::Camera>();
	c->shaderPath = "postProcess/SSAO.PS.hlsl";
	LWP::Object::CreateRenderTexture(c, 640, 360);
	c->transform.translation = { -5.9f,4.5f,-10.0f };
	c->transform.rotation = { 0.4f,0.5f,0.0f };
	// ポストプロセステスト
	c->isUsePostProcess = true;
	c->isActive = true;
	
	mainCamera->isUsePostProcess = true;
	mainCamera->shaderPath = "postProcess/SSAO.PS.hlsl";
	mainCamera->ReCreateShader();
	//SetMainRenderCamera(subCamera);

	// 地面
	Mesh* ground = LWP::Resource::LoadModel("cube/cube.obj");
	ground->transform.translation.y = -1.5f;
	ground->transform.scale = { 10.0f,0.1f, 10.0f };
	//ground->material.enableLighting = true;
	ground->name = "Ground";
	//ground->commonColor = new Color()

	// 三角形
	for (int i = 0; i < 2; i++) {
		tri[i] = LWP::Common::CreateInstance<Triangle>();
	}
	tri[0]->vertices[0].color = Color(RED);
	tri[0]->vertices[1].color = Color(BLUE);
	tri[0]->vertices[2].color = Color(GREEN);
	tri[1]->transform.rotation.y = 1.0f;
	tri[1]->texture = uvTexture;

	// 平面
	surface = LWP::Common::CreateInstance<Surface>();
	surface->transform.translation.x = -0.7f;
	surface->texture = uvTexture;
	surface->isActive = false;

	// 球
	sphere = LWP::Common::CreateInstance<Sphere>();
	sphere->radius = 0.3f;
	sphere->transform.translation.x = -1.0f;
	sphere->transform.translation.z = -1.0f;
	//sphere->material.enableLighting = true;
	sphere->material.shininess = 40.0f;
	sphere->texture = uvTexture;

	// モデル読み込み
	cubeModel = LWP::Resource::LoadModel("cube/cube.obj");
	cubeModel->transform.translation.y = -3.0f;
	cubeModel->transform.scale = { 5.0f,5.0f, 0.05f };
	cubeModel->isActive = true;
	//cubeModel->material.enableLighting = true;
	//stressTestModel = LWP::Resource::LoadModel("RGM-96XJesta_13_td.obj");
	//stressTestModel->material.enableLighting = true;
	//stressTestModel->isActive = true;
	// 左側に壁を置く
	Primitive::Mesh* cube2 = LWP::Resource::LoadModel("cube/cube.obj");
	cube2->transform.translation = { -2.1f, -3.0f, -2.0f };
	cube2->transform.rotation.y = -1.54f;
	cube2->transform.scale = { 5.0f,5.0f, 0.05f };
	cube2->isActive = true;


	// 平行光源
	Object::DirectionLight* dirLight = Object::CreateInstance<Object::DirectionLight>();
	dirLight->isActive = true;
	dirLight->intensity = 0.3f;

	// 点光源
	Object::PointLight* pL1 = Object::CreateInstance<Object::PointLight>();
	pL1->transform.translation = { 1.6f,0.0f,-0.1f };
	pL1->intensity = 0.3f;
	pL1->isActive = true;
	Object::PointLight* pL2 = Object::CreateInstance<Object::PointLight>();
	pL2->transform.translation = { -1.5f,-0.1f,-0.1f };
	pL2->intensity = 0.3f;
	pL2->isActive = true;

	// 複数画面描画の結果を張り付けるスプライト
	Sprite* s = LWP::Primitive::CreateInstance<Sprite>();
	s->texture = subCamera->GetRenderTexture();
	s->isUI = true;
	Sprite* s2 = LWP::Primitive::CreateInstance<Sprite>();
	s2->transform.translation.x = 640.0f;
	s2->texture = c->GetRenderTexture();
	s2->isUI = true;


	// パーティクルテスト
	particle = Object::CreateInstance<Object::Particle>();
	particle->primitive = LWP::Resource::LoadModel("cube/cube.obj");
	particle->initFunction = [](Primitive::IPrimitive* primitive) {
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
	particle->updateFunction = [](Object::ParticleData* data) {
		// 経過フレーム追加
		data->elapsedFrame++;

		data->wtf.translation += data->velocity;	// 速度ベクトルを加算
		data->wtf.rotation += data->velocity;	// ついでに回転させとく
		data->wtf.translation.y += -9.8f / 100.0f;	// 重力を加算

		// 速度ベクトルを弱める
		data->velocity *= 0.9f;

		return data->elapsedFrame > 180 ? true : false;
	};
	particle->isActive = true;

	mainCamera->isUsePostProcess = false;
	subCamera->isUsePostProcess = false;
	c->isUsePostProcess = false;


	Cube* cube =LWP::Primitive::CreateInstance<Cube>();
	cube->texture = monsterBall;


	Collider::AABB* aabbCol = LWP::Common::CreateInstance<Collider::AABB>();
	aabbCol->CreateFromPrimitive(sphere);
	// 試しにラムダ式を入れてみる
	aabbCol->SetOnCollisionLambda([](Collider::HitData data) {
		if (data.state == Collider::OnCollisionState::None) {
			Utility::Log("None\n");
		}
		else {
			Utility::Log("Not None\n");
		}
	});

	Collider::AABB* aabbCol2 = LWP::Common::CreateInstance<Collider::AABB>();
	aabbCol2->Create({2.0f,0.0f,0.0f});

	Collider::Sphere* sphereCol0 = LWP::Common::CreateInstance<Collider::Sphere>();
	sphereCol0->Create({ 3.0f,0.0f,0.0f });
}

// 更新
void SampleTitle::Update() {
	// ポストプロセスの切り替え
	if (Keyboard::GetTrigger(DIK_SPACE)) {
		mainCamera->isUsePostProcess = !mainCamera->isUsePostProcess;
	}

	// Tキーを押すとテクスチャ切り替え
	if (Keyboard::GetTrigger(DIK_T)) {
		if (!useMonsterBall) {
			sphere->texture = monsterBall;
			useMonsterBall = true;
		}
		else {
			sphere->texture = uvTexture;
			useMonsterBall = false;
		}
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
		mainCamera->ReCreateShader();
	}
	// パーティクル呼び出し
	if (Keyboard::GetTrigger(DIK_P)) {
		particle->Add(16);
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
		nextScene_ = new SampleGameScene();
	}
}