#include "ShadowTest.h"

using namespace LWP::Object;
using namespace LWP::Math;

// 初期化
void ShadowTest::Initialize() {
	axis.LoadShortPath("axis.obj");
	axis.worldTF.translation = { 0.0f, 1.0f, -3.0f };
	axis.worldTF.scale = { 0.5f, 0.5f, 0.5f };
	
	field.LoadCube();
	field.worldTF.scale = { 100.0f, 0.2f, 100.0f };
	
	for (int i = 0; i < 5; i++) {
		build[i].LoadShortPath("Building1/Building1.gltf");
		build[i].worldTF.translation.x = (i - 2) * 15.0f;
	}

	mainCamera.worldTF.translation = { 0.0f, 5.0f, -40.0f };
	//GetDirLight()->worldTF.rotation *= Quaternion::CreateFromAxisAngle(Vector3::UnitX(), 1.57f);
}

// 更新
void ShadowTest::Update() {
	ImGui::Begin("ShadowTest");
	LWP::Object::GetDirLight()->DebugGUI();
	if (ImGui::TreeNode("Axis")) {
		axis.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("field")) {
		field.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Build")) {
		for (int i = 0; i < 5; i++) {
			if (ImGui::TreeNode(std::string("Build_" + std::to_string(i)).c_str())) {
				build[i].DebugGUI();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::End();
}