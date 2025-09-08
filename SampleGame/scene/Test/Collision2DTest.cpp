#include "Collision2DTest.h"
#include "NoneScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void Collision2DTest::Initialize() {
	rectangle[0].SetBroadShape<Collider2D::Rectangle>();
	rectangle[1].SetBroadShape<Collider2D::Rectangle>();
	sp_.material.texture = Resource::LoadTextureLongPath("resources/system/texture/Rectangle.png");

	for (int i = 0; i < 2; i++) {
		//circle[i];
	}

	circle[0].worldTF.translation = { 100.0f, 100.0f, 0.0f };
	circle[1].worldTF.translation = { 225.0f, 100.0f, 0.0f };
	rectangle[0].worldTF.translation = { 100.0f, 225.0f, 0.0f };
	rectangle[1].worldTF.translation = { 325.0f, 225.0f, 0.0f };
}

// 更新
void Collision2DTest::Update() {
	ImGui::Begin("Test");
	if (ImGui::TreeNode("Circle0")) {
		circle[0].DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Circle1")) {
		circle[1].DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Rectangle0")) {
		rectangle[0].DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Rectangle1")) {
		rectangle[1].DebugGUI();
		ImGui::TreePop();
	}
	ImGui::End();
}