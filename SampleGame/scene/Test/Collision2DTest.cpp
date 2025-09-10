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
	rectCol[0].SetBroadShape<Collider2D::Rectangle>();
	rectCol[0].mask.SetBelongFrag(ColMask1);
	rectCol[0].mask.SetHitFrag(ColMask1);
	rectCol[1].SetBroadShape<Collider2D::Rectangle>();
	rectCol[1].mask.SetBelongFrag(ColMask1);
	rectCol[1].mask.SetHitFrag(ColMask1);

	// 何個か作っておく
	//for (int i = 0; i < 100; i++) {
	//	Collision2D& c = cols.emplace_back();
	//	c.mask.SetBelongFrag(ColMask0);
	//	c.mask.SetHitFrag(ColMask0);
	//	Vector3 v = Utility::Random::GenerateVector3({ -1.0f,-1.0f,0.0f }, { 1.0f,1.0f,0.1f }).Normalize();
	//	velocities.emplace_back(v);
	//}
}

// 更新
void Collision2DTest::Update() {
	ImGui::Begin("Test");
	if (ImGui::TreeNode("Rectangle")) {
		rectCol[0].DebugGUI();
		rectCol[1].DebugGUI();
		ImGui::TreePop();
	}
	ImGui::Text("Collision Count : %d", cols.size());
	if (ImGui::Button("Summon New Collision")) {
		Collision2D& c = cols.emplace_back();
		c.mask.SetBelongFrag(ColMask0);
		c.mask.SetHitFrag(ColMask0);
		Vector3 v = Utility::Random::GenerateVector3({ -1.0f,-1.0f,0.0f }, { 1.0f,1.0f,0.1f }).Normalize();
		velocities.emplace_back(v);
	}
	ImGui::End();

	const static float kRadius = 50.0f;
	const static float kVelocityMultiply = 5.0f;
	const static float kLeft = 0.0f + kRadius;
	const static float kRight = 1920.0f - kRadius;
	const static float kUp = 0.0f + kRadius;
	const static float kDown = 1080.0f - kRadius;

	auto colItr = cols.begin();
	auto veloItr = velocities.begin();
	for (int i = 0; i < cols.size(); i++) {
		colItr->worldTF.translation += *veloItr * kVelocityMultiply;

		// 画面端で跳ね返る
		if (colItr->worldTF.translation.x < kLeft) {
			colItr->worldTF.translation.x = kLeft,
				veloItr->x *= -1;
		}
		else if (colItr->worldTF.translation.x > kRight) {
			colItr->worldTF.translation.x = kRight,
				veloItr->x *= -1;
		}
		if (colItr->worldTF.translation.y < kUp) {
			colItr->worldTF.translation.y = kUp,
				veloItr->y *= -1;
		}
		else if (colItr->worldTF.translation.y > kDown) {
			colItr->worldTF.translation.y = kDown,
				veloItr->y *= -1;
		}

		colItr++;
		veloItr++;
	}
}