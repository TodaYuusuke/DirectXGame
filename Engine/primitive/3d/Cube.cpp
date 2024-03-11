#include "Cube.h"
#include "base/ImGuiManager.h"
#include "object/collider/cAABB.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

void Cube::CreateVertices() {
	CreateVertices({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f });
}
void Cube::CreateVertices(const LWP::Math::Vector3& min, const LWP::Math::Vector3& max) {
	// 頂点をクリア
	vertices.clear();
	vertices.resize(GetVertexCount());
	// 幅（中心が原点と考える）

	//ーーー 効率的なやり方が思いつかなかったのでごり押しで作る ーーー//
	vertices[0].position = { min.x,max.y,max.z };	//　　　//
	vertices[1].position = { max.x,max.y,max.z };	//　上　//
	vertices[2].position = { min.x,max.y,min.z };	//　部　//
	vertices[3].position = { max.x,max.y,min.z };	//　　　//
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
	vertices[4].position = { min.x,min.y,min.z };	//　　　//
	vertices[5].position = { max.x,min.y,min.z };	//　下　//
	vertices[6].position = { min.x,min.y,max.z };	//　部　//
	vertices[7].position = { max.x,min.y,max.z };	//　　　//
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
	vertices[8].position = { min.x,max.y,min.z };	//　　　//
	vertices[9].position = { max.x,max.y,min.z };	//　前　//
	vertices[10].position = { min.x,min.y,min.z };	//　部　//
	vertices[11].position = { max.x,min.y,min.z };	//　　　//
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
	vertices[12].position = { min.x,max.y,max.z };	//　　　//
	vertices[13].position = { min.x,max.y,min.z };	//　左　//
	vertices[14].position = { min.x,min.y,max.z };	//　部　//
	vertices[15].position = { min.x,min.y,min.z };	//　　　//
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
	vertices[16].position = { max.x,max.y,min.z };	//　　　//
	vertices[17].position = { max.x,max.y,max.z };	//　右　//
	vertices[18].position = { max.x,min.y,min.z };	//　部　//
	vertices[19].position = { max.x,min.y,max.z };	//　　　//
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
	vertices[20].position = { max.x,max.y,max.z };	//　　　//
	vertices[21].position = { min.x,max.y,max.z };	//　後　//
	vertices[22].position = { max.x,min.y,max.z };	//　部　//
	vertices[23].position = { min.x,min.y,max.z };	//　　　//
	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//

	// 共通の値をセット
	for (int i = 0; i < 24; i++) {
		int index = i % 4;
		int x = index & 0b01;	// (　•´ω•` )ﻭﾄﾞﾔｯ
		int y = (index & 0b10) >> 1;
		vertices[i].texCoord = { static_cast<float>(x),static_cast<float>(y) };
		vertices[i].normal = vertices[i].position;
		vertices[i].color = ColorPattern::WHITE;
	}
}

void Cube::CreateIndexes() {
	// インデックスをクリア
	indexes.clear();
	indexes.resize(GetIndexCount());

	int index = 0;
	for (int i = 0; i < GetIndexCount(); i+=6) {
		indexes[i + 0] = index;
		indexes[i + 1] = index + 1;
		indexes[i + 2] = index + 2;
		indexes[i + 3] = index + 1;
		indexes[i + 4] = index + 3;
		indexes[i + 5] = index + 2;
		index += 4;
	}
}

int Cube::GetVertexCount() const { return 24; }
int Cube::GetIndexCount() const { return (GetVertexCount() / 2) * 3; }

void Cube::CreateFromAABB(const LWP::Object::Collider::AABB& aabb) {
	CreateVertices(aabb.min, aabb.max);
}

void Cube::DerivedDebugGUI(const std::string& label) {
	label;
#if DEMO
	static float size = 0.5f;
	ImGui::DragFloat("size", &size, 0.01f);
	if (ImGui::Button("ReSize")) { CreateVertices({-size,-size,-size}, {size,size,size}); }
#endif
}