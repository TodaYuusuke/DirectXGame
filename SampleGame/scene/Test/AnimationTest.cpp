#include "AnimationTest.h"
#include "NoneScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void AnimationTest::Initialize() {
	cube.LoadCube();
	for (int i = 0; i < 5; i++) {
		model[i].LoadShortPath("Player/Robot_FL/Player.gltf");
		model[i].worldTF.translation.x = i * 5.5f;
		anim[i].LoadFullPath("resources/model/Player/Robot_FL/Player.gltf", &model[i]);
	}
	
	mainCamera.worldTF.translation = { 0.0f,6.0f, -50.0f };

	/*
	anim[0].Play("Walk")
		.Loop(true);
	anim[0].Play("Dash", 0.0f, 0.0f, Resource::Animation::TrackType::Blend)
	 	.Loop(true, Resource::Animation::TrackType::Blend);
	*/

	// 再読み込みテスト
	//model[0].LoadShortPath("Player/Hinmin/Player.gltf");
}

// 更新
void AnimationTest::Update() {
	static int I = 0;
	static float F = 0.0f;
	static Vector3 V = Vector3();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Test");
	anim[0].DebugGUI();
	ImGui::End();

	// シーン再読み込み
	if (Keyboard::GetTrigger(DIK_P)) {
		nextSceneFunction = []() { return new AnimationTest(); };
	}
}