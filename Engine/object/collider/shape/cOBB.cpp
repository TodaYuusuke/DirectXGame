#include "cOBB.h"
#include "base/ImGuiManager.h"
#if DEMO
#include "component/Primitive.h"
#include "base/directX/command/CommandManager.h"
#endif

using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Object::Collider;

OBB::OBB() : AABB({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f }) {
	rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(Vector3{ 0.0f,0.0f,0.0f });
}

void OBB::CreateFromPrimitive(LWP::Primitive::IPrimitive* primitive) {
	follow_ = primitive;
	#if DEMO
	// ワールドトランスフォームのペアレントもしておく
	cube->transform.Parent(&follow_.t->transform);
	#endif

	// 回転行列だけ抜き出しておく
	rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(follow_.t->transform.rotation);
	// 初期化
	min = primitive->vertices[0].position;
	max = min;

	// 最小の値と最大の値を求める
	for (const Vertex& vertex : primitive->vertices) {
		const Vector3& v = vertex.position;
		min.x = min.x > v.x ? v.x : min.x;
		min.y = min.y > v.y ? v.y : min.y;
		min.z = min.z > v.z ? v.z : min.z;
		max.x = max.x < v.x ? v.x : max.x;
		max.y = max.y < v.y ? v.y : max.y;
		max.z = max.z < v.z ? v.z : max.z;
	}
}

#if DEMO
void OBB::ShowWireFrame() {
	// isActive切り替え
	cube->isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	cube->commonColor = new Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
}
#endif

void OBB::UpdateShape() {
	// データが変わったら再生成
	if (follow_.t && follow_.GetChanged()) {
		CreateFromPrimitive(follow_.t);
		#if DEMO
		cube->transform.rotation = follow_.t->transform.rotation;
		#endif
	}

#if DEMO
	cube->CreateFromAABB(*this);	// cube再生成
#endif
}