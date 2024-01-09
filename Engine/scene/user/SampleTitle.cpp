#include "SampleTitle.h"
#include "SampleGameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void SampleTitle::Initialize() {
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTextureLongPath("resources/system/texture/uvChecker.png");
	monsterBall = LWP::Resource::LoadTextureLongPath("resources/system/texture/monsterBall.png");

	// 地面
	ground = LWP::Primitive::CreateInstance<Surface>();
	ground->transform.translation.y = -1.5f;
	ground->transform.rotation.x = 1.57f;
	ground->transform.scale = { 10.0f,10.0f, 10.0f };
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
	sphere->Radius(0.2f);
	sphere->transform.translation.x = 0.7f;
	sphere->material.enableLighting = true;
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

	pointLight = Object::CreateInstance<Object::PointLight>();
	pointLight->transform.translation = { 1.6f,0.0f,-0.1f };
	pointLight->color = Utility::Color::KelvinToRGB(2800);
	pointLight->isActive = true;
	pointLight->transform.Parent(&sphere->transform);

	Sprite* s = LWP::Primitive::CreateInstance<Sprite>();
	s->texture = uvTexture;
}

// 更新
void SampleTitle::Update() {
	ImGui::Begin("Test");
	if (ImGui::Button("Create")) {
		Object::CreateInstance<Object::PointLight>()->isActive = true;
	}
	ImGui::End();
		
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

	// ESCキーでデバッグ情報表示切り替え
	if (Keyboard::GetTrigger(DIK_ESCAPE)) {
		Info::ChangeShowDebugGUI();
	}

	// ENTERキーを押すとシーン切り替え
	if (Controller::GetTrigger(0, XBOX_A)) {
		nextScene_ = new SampleGameScene();
	}
}