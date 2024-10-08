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
	// プレイヤー初期化
	drone_.Init(&mainCamera);

	// Terrainのマスク変更
	levelData.terrain->collision.mask.SetBelongFrag(lwpC::Collider::ALL);

	// モデル読み込み
	mesh.LoadShortPath("meshColliderTest/meshColTest.gltf");
	//mesh.ApplyWorldTransform({
	//	{8.0f,8.0f,8.0f},
	//	Quaternion(),
	//	{1.0f,1.0f,1.0f}
	//});
	Collider::Mesh& m = meshCol.SetBroadShape(Collider::Mesh());
	m.Create(&mesh);

	pointCol.SetFollowTarget(drone_.GetWorldTF());
	pointCol.worldTF.translation = { 0.0f, 0.0f, 10.0f };
	pointCol.name = "point";
	pointCol.isMove = true;
	aabbCol.SetFollowTarget(&pointCol.worldTF);
	aabbCol.SetBroadShape(Collider::AABB());
	aabbCol.name = "aabb";
	aabbCol.isMove = true;
}

// 更新
void ColliderTest::Update() {
	drone_.Update();

	ImGui::Begin("Test");
	ImGui::Text("point morton : %d", pointCol.GetMortonNumber());
	ImGui::Text("aabb morton : %d", aabbCol.GetMortonNumber());
	ImGui::End();
}