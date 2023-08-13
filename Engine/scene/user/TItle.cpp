#include "Title.h"
#include "GameScene.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// 三角形の座標
	for (int i = 0; i < 2; i++) {
		tri[i] = LWP::Engine::CreatePrimitiveInstance<Triangle>();

		tri[i]->vertices[0].position = { 0.0f,0.2f,0.0f };
		tri[i]->vertices[1].position = { 0.2f,0.0f,0.0f };
		tri[i]->vertices[2].position = { -0.2f,0.0f,0.0f };

		tri[i]->vertices[0].texCoord = { 0.5f,0.0f };
		tri[i]->vertices[1].texCoord = { 1.0f,1.0f };
		tri[i]->vertices[2].texCoord = { 0.0f,1.0f };

		tri[i]->transform.rotation.y = 1.0f * i;
	
		tri[i]->defaultColor = new Color(WHITE);
	}

	texture = LWP::Engine::CreateTextureInstance("resources/uvChecker.png");
}
// 更新
void Title::Update() {
	for (int i = 0; i < 2; i++) {
		tri[i]->transform.rotation.y += 0.02f;
	}

	ImGui::Begin("Triangles");
	for (int i = 1; i >= 0; i--) {
		std::string str = "tri[" + std::to_string(i) + "]";
		const char* label = str.c_str();
		if (ImGui::CollapsingHeader(label))
		{
			ImGui::DragFloat3("translation", &tri[i]->transform.translation.x, 0.02f);
			ImGui::DragFloat3("rotation", &tri[i]->transform.rotation.x, 0.02f);
			ImGui::DragFloat3("scale", &tri[i]->transform.scale.x, 0.02f);
			if (i == 0) {
				LWP::Base::ImGuiManager::ColorPicker4("color1", tri[i]->vertices[0].color);
				LWP::Base::ImGuiManager::ColorPicker4("color2", tri[i]->vertices[1].color);
				LWP::Base::ImGuiManager::ColorPicker4("color3", tri[i]->vertices[2].color);
			}
			else {
				LWP::Base::ImGuiManager::ColorPicker4("color", *tri[i]->defaultColor);
			}
		}
	}
	ImGui::End();

	// カメラ操作
	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &mainCamera->transform.translation.x, 0.01f);
	ImGui::DragFloat3("rotation", &mainCamera->transform.rotation.x, 0.01f);
	ImGui::End();
}
// 描画
void Title::Draw() {
	for (int i = 0; i < 2; i++) {
		tri[i]->Draw(FillMode::Fill, texture);
	}
}