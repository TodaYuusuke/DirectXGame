#include "AnimationTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void AnimationTest::Initialize() {
	model.LoadShortPath("Robot/Player_Boned_IK.gltf");
	anim.LoadFullPath("resources/model/Robot/Player_Boned_IK.gltf", &model);

}

// 更新
void AnimationTest::Update() {
	ImGui::Begin("Test");
	anim.DebugGUI();
	ImGui::End();
}