#include "ColliderTest.h"
#include "../GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

ColliderTest::ColliderTest() : particle_(5) {}

// 初期化
void ColliderTest::Initialize() {
	// 地形初期化
	//field_.Init(&levelData, &mainCamera);

	// モデル読み込み
	cube_[0].LoadCube();
	cube_[0].ApplyWorldTransform({
		{3.300f, -1.110f, 0.0f},
		{-0.180f, 0.290f, -0.056f, 0.938f},
		{0.5f, 0.5f, 0.5f},
	});
	cube_[1].LoadCube();
	cube_[1].ApplyWorldTransform({
		{0.0f, -0.8f, 0.0f},
		{0.0f, 0.928f, 0.0f, 0.372f},
		{0.8f, 0.8f, 0.8f},
	});
	mesh_.LoadShortPath("level/buildings/4Story_Wide_2Doors_Mat.gltf");
	mesh_.ApplyWorldTransform({
		{0.0f, -1.8f, 0.0f},
		{0.0f, 0.928f, 0.0f, 0.372f},
		{0.5f, 0.5f, 0.5f},
	});
	mesh_.isActive = false;

	particle_.model.LoadCube();
	particle_.SetShaderPath("Blood/Emitter.CS.hlsl", "Blood/Update.CS.hlsl");
	particle_.Add(1, { 0.0f,2.0f,0.0f });
}

// 更新
void ColliderTest::Update() {
	static int particleAmount = 1;
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
	ImGui::DragInt("ParticleAmount", &particleAmount);
	ImGui::End();

	time++;
	if (time > 30) {
		time = 0;
		particle_.Add(particleAmount);
	}
}