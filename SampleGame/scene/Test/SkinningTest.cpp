#include "SkinningTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void SkinningTest::Initialize() {
	model.LoadShortPath("SkinningTest/Player_Boned_IK.gltf");
	anim.LoadFullPath("resources/model/SkinningTest/Player_Boned_IK.gltf", &model);

	model.worldTF.translation.y = -1.0f;
	model.worldTF.rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), 1.57f);
	model.SetAllMaterialLighting(false);
}

// 更新
void SkinningTest::Update() {
}