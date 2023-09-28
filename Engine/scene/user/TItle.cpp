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

		tri[i]->vertices[0].position = { 0.0f,0.4f,0.0f };
		tri[i]->vertices[1].position = { 0.4f,0.0f,0.0f };
		tri[i]->vertices[2].position = { -0.4f,0.0f,0.0f };
		//tri[i]->vertices[3].position = { -0.4f,0.4f,0.0f };

		tri[i]->vertices[0].texCoord = { 0.5f,0.0f };
		tri[i]->vertices[1].texCoord = { 1.0f,1.0f };
		tri[i]->vertices[2].texCoord = { 0.0f,1.0f };

		tri[i]->transform.rotation.y = 1.0f * i;
	}
	tri[0]->vertices[0].color = Color(RED);
	tri[0]->vertices[1].color = Color(BLUE);
	tri[0]->vertices[2].color = Color(GREEN);

	// 平面
	surface = LWP::Engine::CreatePrimitiveInstance<Surface>();
	surface->vertices[0].position = { -0.3f,0.3f,0.0f };
	surface->vertices[1].position = { 0.3f,0.3f,0.0f };
	surface->vertices[2].position = { 0.3f,-0.3f,0.0f };
	surface->vertices[3].position = { -0.3f,-0.3f,0.0f };
	surface->vertices[0].texCoord = { 0.0f,0.0f };
	surface->vertices[1].texCoord = { 1.0f,0.0f };
	surface->vertices[2].texCoord = { 1.0f,1.0f };
	surface->vertices[3].texCoord = { 0.0f,1.0f };
	for (int i = 0; i < 4; i++) {
		surface->vertices[i].normal = { 0.0f,0.0f,-1.0f };
	}
	surface->isUI = false;	// UIとして描画

	// 球
	sphere = LWP::Engine::CreatePrimitiveInstance<Sphere>();
	sphere->Radius(0.2f);
	sphere->transform.translation.x = 0.7f;

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
}