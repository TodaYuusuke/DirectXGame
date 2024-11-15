#include "ParticleTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void ParticleTest::Initialize() {
	// レベルデータ読み込み
	levelData.SetScaleMultiply(10.0f);	// 地形は10倍サイズに
	levelData.LoadShortPath("SampleGameScene.json");

	// パーティクルのモデル設定
	particle_.model.LoadCube();
	particle_.model.worldTF.scale = { 0.1f, 0.1f ,0.1f };
}

// 更新
void ParticleTest::Update() {
	// パーティクル用ImGui
	ImGui::Begin("Test");
	if (ImGui::TreeNode("Model")) {
		particle_.model.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle")) {
		particle_.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::Text("-----------");
	static int particleAmount = 1;
	ImGui::DragInt("ParticleAmount", &particleAmount);
	if (ImGui::Button("Generate Particle")) {
		particle_.Add(particleAmount);
	}
	ImGui::End();
}
