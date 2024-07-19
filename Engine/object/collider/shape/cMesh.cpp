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

void Mesh::Update() {
#if DEMO
	for (auto it = triangles_.begin(); it != triangles_.end(); it++) {
		it->worldTF = *follow_;
		// isActive切り替え
		it->isActive = isShowWireFrame && isActive;
		// 色を白に戻す
		it->material.color = Utility::Color(Utility::ColorPattern::WHITE);
	}
#endif

	// アクティブがOff -> 早期リターン
	//if (!isActive) { return; }
}

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

#if DEMO
		triangles_.emplace_back();
		triangles_.back().name = "Mesh Collider " + std::to_string(i);
		triangles_.back().vertices[0] = vertices->data_[indexes[i]];
		triangles_.back().vertices[1] = vertices->data_[indexes[i + 1]];
		triangles_.back().vertices[2] = vertices->data_[indexes[i + 2]];
		triangles_.back().material.enableLighting = false;
		triangles_.back().isWireFrame = true;
#endif
	}
}

void Mesh::DebugGUI() {
	ICollisionShape::DebugGUI();
}

void Mesh::Hit() {
#if DEMO
	for (auto it = triangles_.begin(); it != triangles_.end(); it++) {
		// hitしているときは色を変える
		it->material.color = Utility::Color(Utility::ColorPattern::WHITE);
	}
#endif
}