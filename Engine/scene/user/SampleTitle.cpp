#include "SampleTitle.h"
#include "SampleGameScene.h"
#include "../Class/Ease/Easing.h"
#include <time.h>

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void SampleTitle::Initialize() {
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTextureLongPath("resources/system/texture/uvChecker.png");
	monsterBall = LWP::Resource::LoadTexture("monsterBall.png");

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
}

// 更新
void SampleTitle::Update() {
#if _DEBUG
	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
#endif

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

	// ENTERキーを押すとシーン切り替え
	if (Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new SampleGameScene();
	}
}