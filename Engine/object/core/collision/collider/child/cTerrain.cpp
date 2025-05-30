#include "cTerrain.h"

#include "resources/model/StaticModel.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Base;

Terrain::Terrain() {}

Terrain::Terrain(const Terrain& other) { *this = other; }

void Terrain::Update() {}
void Terrain::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// そのまま返す
	*minPtr = min_;
	*maxPtr = max_;
}

void Terrain::Create(const Resource::StaticModel& model) {
	// 必要なデータを用意
	const std::vector<uint32_t>& indexes = model.GetModelData()->indexes; // インデックス
	StructuredBuffer<OutputVertexStruct>* vertices = model.vertexBuffer_.get();	// 頂点

	// -- 4分木空間を生成 -- //

	// このモデルを囲む最小のAABB
	min_ = vertices->data_[0].position.xyz();
	max_ = min_;
	for (int i = 0; i < vertices->GetCount(); i++) {
		const Vector4& v = vertices->data_[i].position;
		min_.x = std::min<float>(v.x, min_.x);
		min_.y = std::min<float>(v.y, min_.y);
		min_.z = std::min<float>(v.z, min_.z);
		max_.x = std::max<float>(v.x, max_.x);
		max_.y = std::max<float>(v.y, max_.y);
		max_.z = std::max<float>(v.z, max_.z);
	}

	// このAABBを正立方体に
	Vector3 size = max_ - min_;	// 大きさ
	float maxSize = std::max<float>(std::max<float>(size.x, size.y), size.z);	// 最大の辺の長さを計算
	Vector3 center = (min_ + max_) / 2.0f;	// 中心点
	// 4分木に登録
	quadtree_.centerPosition = Vector2{ center.x,center.z };
	quadtree_.spaceSize = maxSize / 2.0f;	// 半分の長さ

	// -- モデルデータからポリゴンを生成 -- //
	polygonMap_.clear();
	for (int i = 0; i < indexes.size(); i += 3) {
		Terrain::Polygon p;
		p.pos[0] = vertices->data_[indexes[i]].position.xyz();
		p.pos[1] = vertices->data_[indexes[i + 1]].position.xyz();
		p.pos[2] = vertices->data_[indexes[i + 2]].position.xyz();
		p.normal = Vector3::Cross(p.pos[1] - p.pos[0], p.pos[2] - p.pos[1]).Normalize();	// 外積で面の法線を求める

		// ポリゴンを囲むAABBを生成
		Vector3 min = p.pos[0];
		Vector3 max = min;
		for (int j = 0; j < 3; j++) {
			const Vector3& v = p.pos[j];
			min.x = std::min<float>(v.x, min.x);
			min.y = std::min<float>(v.y, min.y);
			min.z = std::min<float>(v.z, min.z);
			max.x = std::max<float>(v.x, max.x);
			max.y = std::max<float>(v.y, max.y);
			max.z = std::max<float>(v.z, max.z);
		}

		polygonMap_[quadtree_.GetMortonNumber(min, max)].push_back(p);	// 求めたモートン序列番号の元に格納
	}
}

void Terrain::DebugGUI() {
	ICollider::DebugGUI();
	ImGui::Text("Bounding Min : %.3f, %.3f, %.3f", min_.x, min_.y, min_.z);
	ImGui::Text("Bounding Max : %.3f, %.3f, %.3f", max_.x, max_.y, max_.z);
}

void Terrain::Hit() {}
