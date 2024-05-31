#include "ColliderTest.h"
#include "GameScene.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;
using namespace LWP::Object::Collider;

// 初期化
void ColliderTest::Initialize() {
	// デバッグ情報表示
	Info::ChangeShowDebugGUI();

	// モデル読み込み
	mesh.LoadShortPath("human/walk.gltf");
	sphere.LoadSphere();

	// 追従
	sphere.worldTF.scale = { 2.0f,2.0f,2.0f };
	aabb.SetFollowTarget(&sphere);
	aabb.enterLambda = [](HitData data) {
		data;
		Info::ChangeShowDebugGUI();
	};
}

// 更新
void ColliderTest::Update() {
}