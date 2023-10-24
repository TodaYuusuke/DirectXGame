#include "Title.h"
#include "GameScene.h"
#include "../Class/Ease/Easing.h"
#include <time.h>

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTexture("uvChecker.png");
	monsterBall = LWP::Resource::LoadTexture("monsterBall.png");

	// 地面
	ground = LWP::Primitive::CreateInstance<Surface>();
	ground->transform.translation.y = -1.5f;
	ground->transform.rotation.x = 1.57f;
	ground->transform.scale = { 10.0f,10.0f, 10.0f };
	ground->material.enableLighting = true;
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

	// 球
	sphere = LWP::Primitive::CreateInstance<Sphere>();
	sphere->Radius(0.2f);
	sphere->transform.translation.x = 0.7f;
	sphere->material.enableLighting = true;
	surface->texture = uvTexture;

	// モデル読み込み
	cubeModel = LWP::Resource::LoadModel("cube/cube.obj");
	cubeModel->transform.scale = { 0.5f,0.5f, 0.5f };
	cubeModel->material.enableLighting = true;
	axisModel = LWP::Resource::LoadModel("axis.obj");
	axisModel->material.enableLighting = true;
}

// 更新
void Title::Update() {
	ImGui::Begin("Primitive");
	ground->DebugGUI("Ground");
	tri[0]->DebugGUI("Tri0");
	tri[1]->DebugGUI("Tri1");
	surface->DebugGUI("Surface");
	sphere->DebugGUI("Sphere");
	cubeModel->DebugGUI("cube");
	axisModel->DebugGUI("axis");
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

	// ENTERキーを押すとシーン切り替え
	if (Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new GameScene();
	}

	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
}
// 描画
void Title::Draw() {}