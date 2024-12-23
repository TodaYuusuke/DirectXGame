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
	// 地形初期化
	field_.Init(&levelData, &mainCamera);

	// モデル読み込み
	mesh.LoadShortPath("test/meshCollider/Rock.gltf");
	mesh.ApplyWorldTransform({
		{8.0f,8.0f,8.0f},
		Quaternion(),
		{10.0f,10.0f,10.0f}
	});
	Collider::Mesh& m = meshCol.SetBroadShape(Collider::Mesh());
	m.Create(&mesh);
	meshCol.mask.SetBelongFrag(lwpC::Collider::FieldObject);
	meshCol.mask.SetHitFrag(lwpC::Collider::Bullet | lwpC::Collider::Particle | lwpC::Collider::Player);

	pointCol.worldTF.translation = { 0.0f, 0.0f, 10.0f };
	pointCol.name = "point";
	pointCol.isMove = true;
	//aabbCol.SetFollowTarget(&pointCol.worldTF);
	aabbCol.SetBroadShape(Collider::AABB());
	aabbCol.mask.SetBelongFrag(lwpC::Collider::None);
	aabbCol.name = "aabb";
	aabbCol.isMove = true;
}

// 更新
void ColliderTest::Update() {
	ImGui::Begin("Test");
	ImGui::Text("point morton : %d", pointCol.GetMortonNumber());
	ImGui::Text("aabb morton : %d", aabbCol.GetMortonNumber());
	ImGui::End();
}