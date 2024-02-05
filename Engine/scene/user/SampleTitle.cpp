#include "SampleTitle.h"
#include "SampleGameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void SampleTitle::Initialize() {
	// デバッグ情報表示
	Info::ChangeShowDebugGUI();

	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTextureLongPath("resources/system/texture/uvChecker.png");
	monsterBall = LWP::Resource::LoadTextureLongPath("resources/system/texture/monsterBall.png");

	// サブカメラを生成
	subCamera = LWP::Object::CreateInstance<LWP::Object::Camera>();
	LWP::Object::CreateRenderTexture(subCamera, 640, 360);
	subCamera->transform.translation = { 5.9f,4.5f,-10.0f };
	subCamera->transform.rotation = { 0.4f,-0.5f,0.0f };
	// ポストプロセステスト
	subCamera->isUsePostProcess = true;
	subCamera->isActive = true;

	// サブカメラを生成
	LWP::Object::Camera* c = LWP::Object::CreateInstance<LWP::Object::Camera>();
	LWP::Object::CreateRenderTexture(c, 640, 360);
	c->transform.translation = { -5.9f,4.5f,-10.0f };
	c->transform.rotation = { 0.4f,0.5f,0.0f };
	// ポストプロセステスト
	//c->isUsePostProcess = true;
	c->isActive = true;
	
	//mainCamera->isUsePostProcess = true;
	//SetMainRenderCamera(subCamera);

	// 地面
	Mesh* ground = LWP::Resource::LoadModel("cube/cube.obj");
	ground->transform.translation.y = -1.5f;
	ground->transform.scale = { 10.0f,0.1f, 10.0f };
	ground->material.enableLighting = true;
	ground->name = "Ground";
	//ground->commonColor = new Color()

	// 三角形
	for (int i = 0; i < 2; i++) {
		tri[i] = LWP::Primitive::CreateInstance<Triangle>();
	}
	tri[0]->vertices[0].color = Color(RED);
	tri[0]->vertices[1].color = Color(BLUE);
	tri[0]->vertices[2].color = Color(GREEN);
	tri[1]->transform.rotation.y = 1.0f;
	tri[1]->texture = uvTexture;

	// 平面
	surface = LWP::Primitive::CreateInstance<Surface>();
	surface->transform.translation.x = -0.7f;
	surface->texture = uvTexture;
	surface->isActive = false;

	// 球
	sphere = LWP::Primitive::CreateInstance<Sphere>();
	sphere->Radius(0.3f);
	sphere->transform.translation.z = -1.0f;
	sphere->material.enableLighting = true;
	sphere->material.shininess = 40.0f;
	sphere->texture = uvTexture;

	// モデル読み込み
	cubeModel = LWP::Resource::LoadModel("cube/cube.obj");
	cubeModel->transform.translation.y = -3.0f;
	cubeModel->transform.scale = { 5.0f,5.0f, 0.05f };
	cubeModel->material.enableLighting = true;
	cubeModel->isActive = true;
	//stressTestModel = LWP::Resource::LoadModel("RGM-96XJesta_13_td.obj");
	//stressTestModel->material.enableLighting = true;
	//stressTestModel->isActive = true;

	// 平行光源
	Object::DirectionLight* dirLight = Object::CreateInstance<Object::DirectionLight>();
	dirLight->isActive = true;
	dirLight->intensity = 0.3f;

	// 点光源
	Object::PointLight* pL1 = Object::CreateInstance<Object::PointLight>();
	pL1->transform.translation = { 1.6f,0.0f,-1.3f };
	pL1->intensity = 0.3f;
	pL1->isActive = true;
	Object::PointLight* pL2 = Object::CreateInstance<Object::PointLight>();
	pL2->transform.translation = { -1.5f,-0.1f,-1.3f };
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
	particle->func = [](Object::ParticleData* data, int frame) {
		data->wtf.translation += data->velocity;	// 速度ベクトルを加算
		data->wtf.rotation += data->velocity;	// ついでに回転させとく
		data->wtf.translation.y += -9.8f / 100.0f;	// 重力を加算

		// 速度ベクトルを弱める
		data->velocity *= 0.9f;

		return frame > 180 ? true : false;
	};
	particle->isActive = true;
}

// 更新
void SampleTitle::Update() {
	// SPACEキーを押すとテクスチャ切り替え
	if (Keyboard::GetTrigger(DIK_SPACE)) {
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
		particle->elapsedFrame = 0;
		// 全部スケールを小さく
		for (int i = 0; i < 16; i++) {
			Object::ParticleData newData{};
			newData.wtf.translation = { 0.0f,0.0f,0.0f };
			newData.wtf.rotation = { 0.0f,0.0f,0.0f };
			newData.wtf.scale = { 0.1f,0.1f,0.1f };

			// 速度ベクトルを生成
			int dir1 = Utility::GenerateRandamNum<int>(-100, 100);
			int dir2 = Utility::GenerateRandamNum<int>(-100, 100);
			int dir3 = Utility::GenerateRandamNum<int>(-100, 100);
			// 発射のベクトル
			Math::Vector3 dir{ dir1 / 100.0f,dir2 / 100.0f, dir3 / 100.0f };
			newData.velocity = dir.Normalize() * 0.2f;

			// パーティクル追加
			particle->data.push_back(newData);
		}
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
	if (Controller::GetTrigger(XBOX_A)) {
		nextScene_ = new SampleGameScene();
	}
}