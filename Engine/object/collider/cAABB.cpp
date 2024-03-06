#include "cAABB.h"
#include "primitive/IPrimitive.h"
#if DEMO
#include "component/Primitive.h"
#include "base/directX/command/CommandManager.h"
#endif

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;
using namespace LWP::Math;

// 移譲コンストラクタ
AABB::AABB() : AABB({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f }) {}

AABB::AABB(const LWP::Math::Vector3& min_, const LWP::Math::Vector3& max_) {
	min = min_;
	max = max_;

#if DEMO
	// 立方体のインスタンスを作成
	cube = LWP::Primitive::CreateInstance<Cube>();
	cube->CreateFromAABB(*this);
	cube->isWireFrame = true;
#endif
}

void AABB::CreateFromPrimitive(IPrimitive* primitive) {
	// ワールドトランスフォームのペアレントもしておく
	follow_ = primitive;
	// アフィン変換行列
	Matrix4x4 matrix = primitive->transform.GetWorldMatrix();
	// 初期化
	min = primitive->vertices[0].position * matrix;
	max = min;

	// 最小の値と最大の値を求める
	for (const Vertex& vertex : primitive->vertices) {
		Vector3&& v = vertex.position * matrix;
		min.x = min.x < v.x ? v.x : min.x;
		min.y = min.y < v.y ? v.y : min.y;
		min.z = min.z < v.z ? v.z : min.z;
		max.x = max.x > v.x ? v.x : max.x;
		max.y = max.y > v.y ? v.y : max.y;
		max.z = max.z > v.z ? v.z : max.z;
	}
}

bool AABB::CheckCollision(AABB* c) {
	if ((min.x <= c->max.x && max.x >= c->min.x) &&
		(min.y <= c->max.y && max.y >= c->min.y) &&
		(min.z <= c->max.z && max.z >= c->min.z)) {
		return true;	// ヒットしているのでtrue
	}

	return false;	// 単純な当たり判定を返す
}
bool AABB::CheckCollision(OBB* c) {
	c;
	return false;	// 未実装
}
bool AABB::CheckCollision(Sphere* c) {
	c;
	return false;	// 未実装
}


#if DEMO
void AABB::ShowWireFrame(Base::CommandManager* manager) {
	if (!isShowWireFrame) { return; }

	// hitしているときは色を変える
	cube->commonColor = new Utility::Color(hitting ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
	// データが変わったらデバッグ用のCubeを再生成
	if (follow_.t && follow_.GetChanged()) {
		CreateFromPrimitive(follow_.t);
		cube->CreateFromAABB(*this);
	}
	manager->SetDrawData(cube);
};
#endif

void AABB::DerivedDebugGUI() {
	ImGui::DragFloat3("min", &min.x, 0.01f);
	ImGui::DragFloat3("max", &max.x, 0.01f);
}

