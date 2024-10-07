#include "cMesh.h"

#include "resources/model/StaticModel.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Base;


// 移譲コンストラクタ
Mesh::Mesh() {}
Mesh::Mesh(const Mesh& other) {
	*this = other;
}

void Mesh::Update() {}
void Mesh::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	StructuredBuffer<OutputVertexStruct>* vertices = model_->vertexBuffer_.get();	// 頂点を取得
	// 最大地点と最小地点を求める
	Vector3 min = vertices->data_[0].position.xyz();
	Vector3 max = min;

	// 全頂点を検証
	for (int i = 0; i < vertices->GetCount(); i++) {
		Vector3 pos = vertices->data_[i].position.xyz();
		// より小さい方をminに
		min.x = std::min<float>(min.x, pos.x);
		min.y = std::min<float>(min.y, pos.y);
		min.z = std::min<float>(min.z, pos.z);
		// より大きい方をmaxに
		max.x = std::max<float>(min.x, pos.x);
		max.y = std::max<float>(min.y, pos.y);
		max.z = std::max<float>(min.z, pos.z);
	}

	// そのまま返すだけ
	*minPtr = min;
	*maxPtr = max;
}

void Mesh::Create(LWP::Resource::StaticModel* model) {
	model_ = model;
	const std::vector<uint32_t>& indexes = model->GetModelData()->indexes; // インデックス
	StructuredBuffer<OutputVertexStruct>* vertices = model->vertexBuffer_.get();	// 頂点

	for (int i = 0; i < indexes.size(); i += 3) {
		TriangleData& d = data.emplace_back();
		d.pos[0] = vertices->data_[indexes[i]].position.xyz();
		d.pos[1] = vertices->data_[indexes[i + 1]].position.xyz();
		d.pos[2] = vertices->data_[indexes[i + 2]].position.xyz();
		d.normal = Vector3::Cross(d.pos[2] - d.pos[0], d.pos[1] - d.pos[0]).Normalize();	// 外積で面の法線を求める
		d.center = (d.pos[0] + d.pos[1] + d.pos[2]) / 3.0f;
	}
}

void Mesh::DebugGUI() {
	ICollider::DebugGUI();
}

void Mesh::Hit() {}