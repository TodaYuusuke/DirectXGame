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
	ImGui::Begin("Triangles");
	for (int i = 1; i >= 0; i--) {
		std::string str = "tri[" + std::to_string(i) + "]";
		const char* label = str.c_str();
		if (ImGui::CollapsingHeader(label))
		{
			ImGui::DragFloat3((str + "translation").c_str(), &tri[i]->transform.translation.x, 0.02f);
			ImGui::DragFloat3((str + "rotation").c_str(), &tri[i]->transform.rotation.x, 0.02f);
			ImGui::DragFloat3((str + "scale").c_str(), &tri[i]->transform.scale.x, 0.02f);
			ImGui::DragFloat3((str + "vertices[0]").c_str(), &tri[i]->vertices[0].position.x, 0.01f);
			ImGui::DragFloat3((str + "vertices[1]").c_str(), &tri[i]->vertices[1].position.x, 0.01f);
			ImGui::DragFloat3((str + "vertices[2]").c_str(), &tri[i]->vertices[2].position.x, 0.01f);
			LWP::Base::ImGuiManager::ColorEdit4((str + "color1").c_str(), tri[i]->vertices[0].color);
			LWP::Base::ImGuiManager::ColorEdit4((str + "color2").c_str(), tri[i]->vertices[1].color);
			LWP::Base::ImGuiManager::ColorEdit4((str + "color3").c_str(), tri[i]->vertices[2].color);
		}
	}
	ImGui::End();

	// 平面
	ImGui::Begin("Surface");
	ImGui::DragFloat3("translation", &surface->transform.translation.x, 0.01f);
	ImGui::DragFloat3("rotation", &surface->transform.rotation.x, 0.01f);
	ImGui::DragFloat3("scale", &surface->transform.scale.x, 0.01f);
	ImGui::DragFloat3("vertices[0]", &surface->vertices[0].position.x, 0.01f);
	ImGui::DragFloat3("vertices[1]", &surface->vertices[1].position.x, 0.01f);
	ImGui::DragFloat3("vertices[2]", &surface->vertices[2].position.x, 0.01f);
	ImGui::DragFloat3("vertices[3]", &surface->vertices[3].position.x, 0.01f);
	LWP::Base::ImGuiManager::ColorEdit4("color0", surface->vertices[0].color);
	LWP::Base::ImGuiManager::ColorEdit4("color1", surface->vertices[1].color);
	LWP::Base::ImGuiManager::ColorEdit4("color2", surface->vertices[2].color);
	LWP::Base::ImGuiManager::ColorEdit4("color4", surface->vertices[3].color);
	ImGui::End();

	// 球
	ImGui::Begin("Sphere");
	ImGui::DragFloat3("translation", &sphere->transform.translation.x, 0.01f);
	ImGui::DragFloat3("rotation", &sphere->transform.rotation.x, 0.01f);
	ImGui::DragFloat3("scale", &sphere->transform.scale.x, 0.01f);
	float radius = sphere->Radius();
	ImGui::DragFloat("radius", &radius, 0.01f);
	sphere->Radius(radius);
	ImGui::Checkbox("useMonsterBall", &useMonsterBall);
	ImGui::End();

	// カメラ操作
	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &mainCamera->transform.translation.x, 0.01f);
	ImGui::DragFloat3("rotation", &mainCamera->transform.rotation.x, 0.01f);
	ImGui::End();
}
// 描画
void Title::Draw() {
	tri[0]->Draw(FillMode::Fill);
	tri[1]->Draw(FillMode::Fill, uvTexture);

	surface->Draw(FillMode::Fill, uvTexture);
	if (useMonsterBall) { sphere->Draw(FillMode::Fill, monsterBall); }
	else { sphere->Draw(FillMode::Fill, uvTexture); }
}