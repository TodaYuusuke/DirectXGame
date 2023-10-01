#include "Title.h"
#include "GameScene.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// 三角形
	for (int i = 0; i < 2; i++) {
		tri[i] = LWP::Engine::CreatePrimitiveInstance<Triangle>();
		tri[i]->isActive = false;
	}
	tri[0]->vertices[0].color = Color(RED);
	tri[0]->vertices[1].color = Color(BLUE);
	tri[0]->vertices[2].color = Color(GREEN);
	tri[1]->transform.rotation.y = 1.0f;

	// 平面
	surface = LWP::Engine::CreatePrimitiveInstance<Surface>();
	surface->transform.translation.x = -0.7f;
	surface->isActive = false;

	// 球
	sphere = LWP::Engine::CreatePrimitiveInstance<Sphere>();
	sphere->Radius(0.2f);
	sphere->transform.translation.x = 0.7f;
	sphere->isActive = false;


	// モデル読み込み
	planeModel = LWP::Engine::CreateModelInstance("plane.obj");
	axisModel = LWP::Engine::CreateModelInstance("axis.obj");

	uvTexture = LWP::Engine::CreateTextureInstance("resources/uvChecker.png");
	monsterBall = LWP::Engine::CreateTextureInstance("resources/monsterBall.png");
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

	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
}
// 描画
void Title::Draw() {
	tri[0]->Draw();
	tri[1]->Draw(uvTexture);

	surface->Draw(uvTexture);
	if (useMonsterBall) { sphere->Draw(monsterBall); }
	else { sphere->Draw(uvTexture); }

	planeModel->Draw(uvTexture);
	axisModel->Draw(uvTexture);
} 