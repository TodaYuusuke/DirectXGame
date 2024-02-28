#include "cAABB.h"
#include "primitive/IPrimitive.h"
#if DEMO
#include "base/directX/command/CommandManager.h"
#endif

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;

// 形状から包み込む最小のAABBを生成する関数
void AABB::CreateFromPrimitive(IPrimitive* primitive) {
	// ワールドトランスフォームのペアレントもしておく
	followPtr_ = &primitive->transform;
	// 最小の値と最大の値を求める
	for (const Vertex& v : primitive->vertices) {
		min.x = std::min<float>(min.x, v.position.x);
		min.y = std::min<float>(min.y, v.position.y);
		min.z = std::min<float>(min.z, v.position.z);
		max.x = std::max<float>(max.x, v.position.x);
		max.y = std::max<float>(max.y, v.position.y);
		max.z = std::max<float>(max.z, v.position.z);
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

void AABB::ShowWireFrame(Base::CommandManager* manager) {
	manager;
	//manager->SetDrawData();
};

void AABB::DerivedDebugGUI() {

}

