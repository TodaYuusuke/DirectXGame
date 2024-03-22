#include "cAABB.h"
#include "base/ImGuiManager.h"
#if DEMO
#include "component/Primitive.h"
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
	cube = new Cube();
	cube->CreateFromAABB(*this);
	cube->isWireFrame = true;
#endif
}

void AABB::Create(const LWP::Math::Vector3& position) { Create(position, { 1.0f,1.0f,1.0 }); }
void AABB::Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size) {
	// サイズの値を求める
	LWP::Math::Vector3 s = size / 2.0f;
	min = (s * -1.0f) + position; 
	max = s + position;
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
		min.x = min.x > v.x ? v.x : min.x;
		min.y = min.y > v.y ? v.y : min.y;
		min.z = min.z > v.z ? v.z : min.z;
		max.x = max.x < v.x ? v.x : max.x;
		max.y = max.y < v.y ? v.y : max.y;
		max.z = max.z < v.z ? v.z : max.z;
	}
}

#if DEMO
void AABB::ShowWireFrame() {
	// isActive切り替え
	cube->isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	cube->commonColor = new Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
};
#endif

void AABB::UpdateShape() {
	// MinよりMaxのほうが小さくならないように修正
	min.x = std::min<float>(min.x, max.x);
	min.y = std::min<float>(min.y, max.y);
	min.z = std::min<float>(min.z, max.z);

	max.x = std::max<float>(min.x, max.x);
	max.y = std::max<float>(min.y, max.y);
	max.z = std::max<float>(min.z, max.z);
	
	
	// データが変わったら再生成
	if (follow_.t && follow_.GetChanged()) {
		CreateFromPrimitive(follow_.t);
	}

	#if DEMO
	cube->CreateFromAABB(*this);	// cube再生成
	#endif
}

void AABB::DerivedDebugGUI() {
	ImGui::DragFloat3("min", &min.x, 0.01f);
	ImGui::DragFloat3("max", &max.x, 0.01f);
}