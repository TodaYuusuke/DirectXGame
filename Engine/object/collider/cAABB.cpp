#include "cAABB.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;


// 移譲コンストラクタ
AABB::AABB() : AABB({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f }) {}

AABB::AABB(const LWP::Math::Vector3& min_, const LWP::Math::Vector3& max_) {
	min = min_;
	max = max_;

#if DEMO
	// 立方体のインスタンスを作成
	cube.CreateFromAABB(*this);
	cube.material.enableLighting = false;
	cube.isWireFrame = true;
#endif
}

void AABB::Update() {
	ICollider::Update();
#if DEMO
	cube.CreateFromAABB(*this);	// cube再生成
	// isActive切り替え
	cube.isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	cube.material.color = Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	if (!isActive) { return; }


	// MinよりMaxのほうが小さくならないように修正
	min.x = std::min<float>(min.x, max.x);
	min.y = std::min<float>(min.y, max.y);
	min.z = std::min<float>(min.z, max.z);

	max.x = std::max<float>(min.x, max.x);
	max.y = std::max<float>(min.y, max.y);
	max.z = std::max<float>(min.z, max.z);

	// データが変わったら再生成
	if (followModel_.t && followModel_.GetChanged()) {
		// 再生成
		Create(followModel_.t);
	}
}

void AABB::DebugGUI() {
	ICollider::DebugGUI();
	ImGui::DragFloat3("min", &min.x, 0.01f);
	ImGui::DragFloat3("max", &max.x, 0.01f);
}

void AABB::Create(const LWP::Math::Vector3& position) { Create(position, { 1.0f,1.0f,1.0 }); }
void AABB::Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size) {
	// サイズの値を求める
	LWP::Math::Vector3 s = size / 2.0f;
	min = (s * -1.0f) + position; 
	max = s + position;
}

void AABB::Create(LWP::Resource::RigidModel* model) { 
	// 必要なデータを集める
	Matrix4x4 matrix = model->worldTF.GetAffineMatrix();	// アフィン変換行列
	std::vector<Vertex> vertices = model->GetModelData()->GetVertices();
	// 初期化
	min = vertices[0].position * matrix;
	max = min;

	// 最小の値と最大の値を求める
	for (const Vertex& vertex : vertices) {
		Vector3&& v = vertex.position * matrix;
		min.x = min.x > v.x ? v.x : min.x;
		min.y = min.y > v.y ? v.y : min.y;
		min.z = min.z > v.z ? v.z : min.z;
		max.x = max.x < v.x ? v.x : max.x;
		max.y = max.y < v.y ? v.y : max.y;
		max.z = max.z < v.z ? v.z : max.z;
	}
}
