#include "cAABB.h"

#include "resources/model/RigidModel.h"
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
	dPri.ChangeFillMode();	// ワイヤーフレームに
	dPri.SetAllMaterialLighting(false);
#endif
}

void AABB::Update() {
#if DEMO
	Vector3 worldPos = tf_->GetWorldPosition();
	Vector3 worldMin = min + worldPos;
	Vector3 worldMax = max + worldPos;
	dPri.CreateFromAABB(worldMin, worldMax);
	// isActive切り替え
	dPri.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	dPri.materials["Material0"].color = Utility::Color(Utility::ColorPattern::WHITE);
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
}
void AABB::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// そのまま返すだけ
	AABB::Data d(*this);
	*minPtr = d.min;
	*maxPtr = d.max;
}

void AABB::Create(const LWP::Math::Vector3& position) { Create(position, { 1.0f,1.0f,1.0 }); }
void AABB::Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size) {
	// サイズの値を求める
	LWP::Math::Vector3 s = size / 2.0f;
	min = (s * -1.0f) + position;
	max = s + position;
}

void AABB::Create(const LWP::Resource::RigidModel& model) {
	// 必要なデータを集める
	Matrix4x4 matrix = model.worldTF.GetAffineMatrix();	// アフィン変換行列
	std::vector<Vertex> vertices = model.GetModelData()->GetVertices();
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

	// そのモデルを包み込むAABBを生成するだけなので、移動分は戻す
	Vector3 worldPos = model.worldTF.GetWorldPosition();
	min -= worldPos;
	max -= worldPos;
}

void AABB::DebugGUI() {
	ImGui::DragFloat3("position", &tf_->translation.x, 0.01f);
	ImGui::DragFloat3("min", &min.x, 0.01f);
	ImGui::DragFloat3("max", &max.x, 0.01f);
	ICollider::DebugGUI();
}

void AABB::Hit() {
#if DEMO
	// hitしているときは色を変える
	dPri.materials["Material0"].color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

AABB::Data::Data(AABB& aabb) {
	//Matrix4x4 affine = aabb.follow_->GetScaleMatrix() * aabb.follow_->GetTranslationMatrix();
	//min = aabb.min * affine;
	//max = aabb.max * affine;
	Vector3 worldPos = aabb.tf_->GetWorldPosition();
	min = aabb.min + worldPos;
	max = aabb.max + worldPos;
	center = (min + max) / 2.0f;
}