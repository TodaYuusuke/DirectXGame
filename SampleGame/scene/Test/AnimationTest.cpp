#include "AnimationTest.h"
#include "TextureLoadTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

AnimationTest::AnimationTest() :
	caps(col.SetBroadShape(Collider::Capsule())) {

}
// 初期化
void AnimationTest::Initialize() {
	// NormalSpriteの初期化
	normalSprite.LoadTexture("uvChecker.dds");
	// SequenceSpriteの初期化
	sequenceSprite.LoadTexture("TestSpriteSheet.png");
	sequenceSprite.SetSplitSize({ 64.0f, 64.0f });		// 分割するサイズを設定する
	// ClipSpriteの初期化
	clipSprite.LoadTexture("Attack.png");

	normalBill.LoadTexture("Particle.png");		// 通常
	clipBill.LoadTexture("Particle.png");// クリップ
	
	//normalBill.LoadTexture("uvChecker.dds");		// 通常
	//sequenceBill.LoadTexture("TestSpriteSheet.png");	// シーケンス
	//sequenceBill.SetSplitSize({ 64.0f, 64.0f });		// 分割するサイズを設定する
	//clipBill.LoadTexture("Attack.png");// クリップ

	normalSprite.worldTF.translation = { 100.0f, 400.0f, 0.0f };
	sequenceSprite.worldTF.translation = { 400.0f, 200.0f, 0.0f };
	clipSprite.worldTF.translation = { 1000.0f, 600.0f, 0.0f };

	cube.LoadCube();
	for (int i = 0; i < 5; i++) {
		model[i].LoadShortPath("Player/Hinmin/Player.gltf");
		//model[i].LoadShortPath("test/Test.gltf");
		model[i].worldTF.translation.x = i * 1.5f;
		//motion[i].Add(
		//	&model[i].worldTF.translation,
		//	{ -2.526f, 0.0f, 0.0f },
		//	0.0f, 2.0f
		//);
		//motion[i].Add(
		//	&model[i].worldTF.translation,
		//	{ 2.526f, 0.0f, 0.0f },
		//	2.0f, 2.0f
		//);
		////motion[i].Start(true);

		anim[i].LoadFullPath("resources/model/Player/Hinmin/Player.gltf", &model[i]);
		//anim[i].LoadFullPath("resources/model/test/Test.gltf", &model[i]);
	}

	anim[0].Play("Walk")
		.Loop(true);
	anim[0].Play("Dash", 0.0f, 0.0f, Resource::Animation::TrackType::Blend)
	 	.Loop(true, Resource::Animation::TrackType::Blend);

	weapon.LoadShortPath("Player/Weapon/SimpleWeapon.gltf");
	weapon.GetJoint("Grip")->localTF.Parent(&model[0], "WeaponAnchor");

	caps.radius = 0.05f;
	col.SetFollow(&weapon, "Grip");
	//col.SetFollowTarget(&model[0].worldTF);
}

// 更新
void AnimationTest::Update() {
	static int I = 0;
	static float F = 0.0f;
	static Vector3 V = Vector3();

	ImGui::Begin("Test");
	for (int i = 0; i < 1; i++) {
		if (ImGui::TreeNode(std::to_string(i).c_str())) {
			if(ImGui::TreeNode("Model")) {
				model[i].DebugGUI();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Anim")) {
				anim[i].DebugGUI();
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	if (ImGui::TreeNode("Collision")) {
		col.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::End();

	//Vector3 v = { 0.0f,0.0f,0.0f };
	//caps.start = model[0].GetJointWorldPosition("Hand.R");

	if (Keyboard::GetTrigger(DIK_P)) {
		nextSceneFunction = []() {
			return new TextureLoadTest();
		};
	}
}