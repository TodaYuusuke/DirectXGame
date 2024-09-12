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

	audio.LoadShortPath("powerup01.mp3");
}

// 更新
void SkinningTest::Update() {
	if (Keyboard::GetTrigger(DIK_A)) {
		audio.Play(0.5f, 255);
	}
	if (Keyboard::GetTrigger(DIK_S)) {
		audio.Stop();
	}
}