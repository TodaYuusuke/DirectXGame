#include "SkinningTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void SkinningTest::Initialize() {
	for (int i = 0; i < 2; i++) {
		model[i].LoadShortPath("SkinningTest/Player_Boned_IK.gltf");
		anim[i].LoadFullPath("resources/model/SkinningTest/Player_Boned_IK.gltf", &model[i]);
		anim[i].name = "Anim" + std::to_string(i);

		model[i].worldTF.translation.y = -1.0f;
		model[i].worldTF.rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), 1.57f);
		model[i].SetAllMaterialLighting(false);
	}

	model[0].worldTF.translation.x = -1.0f;
	model[0].worldTF.translation.x = 1.0f;

	sp.isUI = true;

	part.model.LoadCube();
}

// 更新
void SkinningTest::Update() {
	ImGui::Begin("Test");
	part.DebugGUI();
	if (ImGui::Button("Generate")) { part.Add(12); }
	ImGui::End();
}