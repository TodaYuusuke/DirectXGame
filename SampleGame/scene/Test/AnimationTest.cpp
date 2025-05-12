#include "AnimationTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void AnimationTest::Initialize() {
	for (int i = 0; i < 5; i++) {
		model[i].LoadShortPath("Player/Hinmin/Player_Simple.gltf");
		model[i].worldTF.translation.x = i * 1.5f;
		motion[i].Add(
			&model[i].worldTF.translation,
			{ -2.526f, 0.0f, 0.0f },
			0.0f, 2.0f
		);
		motion[i].Add(
			&model[i].worldTF.translation,
			{ 2.526f, 0.0f, 0.0f },
			2.0f, 2.0f
		);
		//motion[i].Start(true);

		anim[i].LoadFullPath("resources/model/Player/Hinmin/Player_Simple.gltf", &model[i]);
	}
}

// 更新
void AnimationTest::Update() {
	ImGui::Begin("Test");
	for (int i = 0; i < 5; i++) {
		if (ImGui::TreeNode(std::to_string(i).c_str())) {
			if(ImGui::TreeNode("Model")) {
				model[i].DebugGUI();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Anim")) {
				anim[i].DebugGUI();
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}