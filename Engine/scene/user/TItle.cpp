#include "Title.h"
#include "GameScene.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTexture("uvChecker.png");
	monsterBall = LWP::Resource::LoadTexture("monsterBall.png");

	LWP::Input::GetTrigger(DIK_0);

	// 三角形
	for (int i = 0; i < 2; i++) {
		tri[i] = LWP::Primitive::CreateInstance<Triangle>();
		tri[i]->isActive = false;
	}
	tri[0]->vertices[0].color = Color(RED);
	tri[0]->vertices[1].color = Color(BLUE);
	tri[0]->vertices[2].color = Color(GREEN);
	tri[1]->transform.rotation.y = 1.0f;
	tri[1]->texture = uvTexture;

	// 平面
	surface = LWP::Primitive::CreateInstance<Surface>();
	surface->transform.translation.x = -0.7f;
	surface->isActive = true;
	surface->texture = uvTexture;

	// 球
	sphere = LWP::Primitive::CreateInstance<Sphere>();
	sphere->Radius(0.2f);
	sphere->transform.translation.x = 0.7f;
	sphere->isActive = true;
	surface->texture = uvTexture;

	// モデル読み込み
	planeModel = LWP::Resource::LoadModel("plane.obj");
	axisModel = LWP::Resource::LoadModel("axis.obj");
}
// 更新
void Title::Update() {
	ImGui::Begin("Primitive");
	tri[0]->DebugGUI("Tri0");
	tri[1]->DebugGUI("Tri1");
	surface->DebugGUI("Surface");
	sphere->DebugGUI("Sphere");
	planeModel->DebugGUI("plane");
	axisModel->DebugGUI("axis");
	ImGui::Checkbox("useMonsterBall", &useMonsterBall);
	ImGui::End();

	// SPACEキーを押すとテクスチャ切り替え
	if (LWP::Engine::GetKeyStateTrigger(DIK_SPACE)) {
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
	if (LWP::Engine::GetKeyStateTrigger(DIK_RETURN)) {
		nextScene_ = new GameScene();
	}


	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
}
// 描画
void Title::Draw() {
	//tri[0]->Draw();
	//tri[1]->Draw();
	//surface->Draw();
	//sphere->Draw();

	//planeModel->Draw();
	//axisModel->Draw();
} 