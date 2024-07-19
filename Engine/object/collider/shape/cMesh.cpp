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

void Mesh::Create(const LWP::Resource::StaticModel& model) {
	const std::vector<uint32_t>& indexes = model.GetModelData()->indexes; // インデックス
	StructuredBuffer<OutputVertexStruct>* vertices = model.vertexBuffer_.get();	// 頂点

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
	ICollisionShape::DebugGUI();
}

void Mesh::Hit() {}