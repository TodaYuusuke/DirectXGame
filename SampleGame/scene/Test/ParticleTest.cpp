#include "ParticleTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void ParticleTest::Initialize() {
	dirt_.sprite.worldTF.translation = { 100.0f,100.0f,0.0f };
	tracesOfDirt_.sprite.worldTF.translation = { 100.0f,100.0f,0.0f };
}

// 更新
void ParticleTest::Update() {
	if (Keyboard::GetTrigger(DIK_SPACE)) {
		tracesOfDirt_.Add(10);
	}
	
	// パーティクル用ImGui
	ImGui::Begin("Test");
	if (ImGui::TreeNode("Dirt")) {
		dirt_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("TracesOfDirt")) {
		tracesOfDirt_.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::End();
}
