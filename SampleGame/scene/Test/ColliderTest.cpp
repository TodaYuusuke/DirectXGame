#include "ColliderTest.h"
#include "../GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

ColliderTest::ColliderTest() : blood_(100), deadBody_(100), bloodFall_(100) {}

// 初期化
void ColliderTest::Initialize() {
	// 地形初期化
	//field_.Init(&levelData, &mainCamera);

	// モデル読み込み
	cube_[0].LoadCube();
	cube_[0].ApplyWorldTransform({
		{3.300f, -1.110f, 0.0f},
		{0.0f, 0.928f, 0.0f, 0.372f},
		{0.5f, 0.5f, 0.5f},
	});
	cube_[1].LoadCube();
	cube_[1].ApplyWorldTransform({
		{0.0f, -0.8f, 0.0f},
		{-0.180f, 0.290f, -0.056f, 0.938f},
		{0.8f, 0.8f, 0.8f},
	});
	mesh_.LoadShortPath("level/buildings/4Story_Wide_2Doors_Mat.gltf");
	mesh_.ApplyWorldTransform({
		{0.0f, -1.8f, 0.0f},
		{0.0f, 0.928f, 0.0f, 0.372f},
		{0.5f, 0.5f, 0.5f},
	});
	mesh_.isActive = false;

	blood_.model.LoadCube();
	blood_.SetShaderPath("Blood/Emitter.CS.hlsl", "Blood/Update.CS.hlsl", "Blood/Hit.CS.hlsl");
	blood_.Add(1, { 0.0f,2.0f,0.0f });
	deadBody_.model.LoadCube();
	deadBody_.SetShaderPath("DeadBody/Emitter.CS.hlsl", "DeadBody/Update.CS.hlsl", "DeadBody/Hit.CS.hlsl");
	deadBody_.Add(1, { 0.0f,2.0f,0.0f });
	bloodFall_.model.LoadCube();
	bloodFall_.SetShaderPath("BloodFall/Emitter.CS.hlsl", "BloodFall/Update.CS.hlsl", "BloodFall/Hit.CS.hlsl");
	bloodFall_.Add(1, { 0.0f,2.0f,0.0f });
}

// 更新
void ColliderTest::Update() {
	// パーティクルの継続数
	static int particleAmount = 1024;
	// パーティクルの継続生成間隔
	static int interval = 10;

	// パーティクル用ImGui
	ImGui::Begin("Test");
	if (ImGui::TreeNode("Blood")) {
		blood_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("DeadBody")) {
		deadBody_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("BloodFall")) {
		bloodFall_.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::Text("-----------");
	ImGui::DragInt("ParticleAmount", &particleAmount);
	ImGui::DragInt("Interval", &interval);
	ImGui::Checkbox("Add_Blood", &addBlood);
	ImGui::Checkbox("Add_DeadBody", &addDeadBody);
	ImGui::Checkbox("Add_BloodFall", &addBloodFall);
	ImGui::End();

	time++;
	if (time > interval) {
		time = 0;
		if (addBlood) { blood_.Add(particleAmount); }
		if (addDeadBody) { deadBody_.Add(particleAmount); }
		if (addBloodFall) { bloodFall_.Add(particleAmount); }
	}
}