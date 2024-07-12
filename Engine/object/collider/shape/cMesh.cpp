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

#if DEMO
	//cube.material.enableLighting = false;
	//cube.isWireFrame = true;
#endif
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
	StructuredBuffer<uint32_t>* indexes = model.GetModelData()->buffers_.primitiveIndices.get();
	StructuredBuffer<OutputVertexStruct>* vertices = model.vertexBuffer_.get();

	for (int i = 0; i < indexes->GetCount(); i += 3) {
#if DEMO
		triangles_.emplace_back();
		triangles_.back().vertices[0] = vertices->data_[indexes->data_[i]];
		triangles_.back().vertices[1] = vertices->data_[indexes->data_[i + 1]];
		triangles_.back().vertices[2] = vertices->data_[indexes->data_[i + 2]];
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

Mesh::Data::Data(Mesh& aabb) {
	aabb;
}