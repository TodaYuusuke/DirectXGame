#include "Collision3DTest.h"
#include "NoneScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void Collision3DTest::Initialize() {
}

// 更新
void Collision3DTest::Update() {
	ImGui::Begin("Test");
	//if (ImGui::TreeNode("Rectangle")) {
	//	ImGui::TreePop();
	//}
	ImGui::End();
}