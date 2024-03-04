#include "cAABB.h"
#include "primitive/IPrimitive.h"
#if DEMO
#include "component/Primitive.h"
#include "base/directX/command/CommandManager.h"
#endif

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;
using namespace LWP::Math;

// 形状から包み込む最小のAABBを生成する関数
void AABB::CreateFromPrimitive(IPrimitive* primitive) {
	// ワールドトランスフォームのペアレントもしておく
	follow_ = primitive;
	// アフィン変換行列
	Matrix4x4 matrix = primitive->transform.GetWorldMatrix();
	// 初期化
	min = primitive->vertices[0].position * matrix;
	max = primitive->vertices[0].position * matrix;

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
	c;
	return false;	// 未実装
}
bool AABB::CheckCollision(OBB* c) {
	c;
	return false;	// 未実装
}
bool AABB::CheckCollision(Sphere* c) {
	c;
	return false;	// 未実装
}

AABB::AABB(const LWP::Math::Vector3& min_, const LWP::Math::Vector3& max_) {
	min = min_;
	max = max_;
}

#if DEMO
void AABB::ShowWireFrame(Base::CommandManager* manager) {
	// 立方体のインスタンスを作成
	static Cube* cube = LWP::Primitive::CreateInstance<Cube>();
	cube->CreateFromAABB(*this);
	cube->isWireFrame = true;
	cube->commonColor = new Utility::Color(hitting ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
	if (follow_.GetChanged()) {
		//cube->transform.translation = follow_.t->transform.translation;
		//cube->transform.scale = follow_.t->transform.scale;
		CreateFromPrimitive(follow_.t);
	}
	manager->SetDrawData(cube);
};
#endif

void AABB::DerivedDebugGUI() {
	ImGui::DragFloat3("min", &min.x, 0.01f);
	ImGui::DragFloat3("max", &max.x, 0.01f);
}

