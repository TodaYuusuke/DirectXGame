#include "ColliderTest.h"
#include "../GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void ColliderTest::Initialize() {
	// デバッグ情報表示
	//Info::ChangeShowDebugGUI();

	// モデル読み込み
	mesh.LoadShortPath("meshColliderTest/meshColTest.gltf");
	//mesh.LoadShortPath("ground/Ground.gltf");
	
	Collider::Mesh& m = meshCol.SetBroadShape(Collider::Mesh());
	m.Create(mesh);

	pointCol.isMove = true;
	pointCol.stayLambda = [&](Collision* c) {
		ImGui::Begin("Test");
		ImGui::End();
	};
	//// 追従
	//aabb.SetFollowTarget(&mesh);
	//aabb.Create(&mesh);
	//aabb.enterLambda = [](HitData data) {
	//	data;
	//	Info::ChangeShowDebugGUI();
	//};
}

// 更新
void ColliderTest::Update() {

	if (Keyboard::GetPress(DIK_W)) {
		pointCol.worldTF.translation.z += 0.01f;
	}
	if (Keyboard::GetPress(DIK_S)) {
		pointCol.worldTF.translation.z -= 0.01f;
	}
	if (Keyboard::GetPress(DIK_A)) {
		pointCol.worldTF.translation.x -= 0.01f;
	}
	if (Keyboard::GetPress(DIK_D)) {
		pointCol.worldTF.translation.x += 0.01f;
	}
	if (Keyboard::GetPress(DIK_SPACE)) {
		pointCol.worldTF.translation.y += 0.01f;
	}
	if (Keyboard::GetPress(DIK_LSHIFT)) {
		pointCol.worldTF.translation.y -= 0.01f;
	}

	if (Keyboard::GetPress(DIK_SPACE)) {
		pointCol.worldTF.translation.y += 0.01f;
	}
	if (Keyboard::GetPress(DIK_LSHIFT)) {
		pointCol.worldTF.translation.y -= 0.01f;
	}
}