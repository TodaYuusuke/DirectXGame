#include "Sphere.h"
#include "base/ImGuiManager.h"
#include "object/core/collision/collider/child/cSphere.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

void Sphere::CreateVertices() {
	// 頂点をクリア
	vertices.clear();
	vertices.resize(GetVertexCount());

	int arrayIndex = 0;
	
	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex <= subdivision; ++latIndex) {
		float lat = (float)(-M_PI / 2.0f + GetLatEvery() * latIndex); // 現在の緯度
		// 緯度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex <= subdivision; ++lonIndex) {
			float lon = lonIndex * GetLonEvery(); // 現在の経度
			// world座標系での点を求める
			vertices[arrayIndex].position =
			{ cosf(lat) * cosf(lon) * radius, sinf(lat) * radius, cosf(lat) * sinf(lon) * radius };

			// UV座標系をセット
			float u = float(lonIndex) / float(subdivision);
			float v = 1.0f - float(latIndex) / float(subdivision);
			vertices[arrayIndex].texCoord = { u,v };

			// 法線をセット
			vertices[arrayIndex].normal = vertices[arrayIndex].position;

			// 色をセット
			vertices[arrayIndex].color = ColorPattern::WHITE;	// 初期カラー白を代入

			// 配列ずらし
			arrayIndex++;
		}
	}
}

void Sphere::CreateIndexes() {
	// インデックスをクリア
	indexes.clear();
	indexes.resize(GetIndexCount());

	int arrayIndex = 0;
	// 一番下の頂点
	for (uint32_t x = 0; x < subdivision; x++) {
		indexes[arrayIndex++] = x;
		indexes[arrayIndex++] = x + subdivision + 1;
		indexes[arrayIndex++] = x + subdivision + 2;
	}

	for (uint32_t y = 1; y <= subdivision - 2u; y++) {
		for (uint32_t x = 0; x < subdivision; x++) {
			indexes[arrayIndex++] = (y * (subdivision + 1u)) + x;
			indexes[arrayIndex++] = ((y + 1) * (subdivision + 1u)) + x;
			indexes[arrayIndex++] = (y * (subdivision + 1u)) + (x + 1);
			indexes[arrayIndex++] = ((y + 1) * (subdivision + 1u)) + x;
			indexes[arrayIndex++] = ((y + 1) * (subdivision + 1u)) + (x + 1);
			indexes[arrayIndex++] = (y * (subdivision + 1u)) + (x + 1);
		}
	}

	// 一番上の頂点
	for (uint32_t x = 0; x < subdivision; x++) {
		indexes[arrayIndex++] = subdivision * subdivision + x - 1u;
		indexes[arrayIndex++] = (subdivision * (subdivision + 1u)) + x;
		indexes[arrayIndex++] = subdivision * subdivision + x;
	}
}

void Sphere::Update() {
	// 値が更新されていたら再生成
	if (GetChanged()) {
		CreateVertices();
		CreateIndexes();
	}
};


int Sphere::GetVertexCount() const { return (subdivision + 1u) * (subdivision + 1u); }
int Sphere::GetIndexCount() const { return subdivision * (subdivision - 1u) * 2u * 3u; }

void Sphere::CreateFromSphereCol(const Math::Vector3& pos, const float rad) {
	worldTF.translation = pos;
	radius = rad;
	subdivision = 16;
	CreateVertices();	// 再計算
	CreateIndexes();
}

void Sphere::DerivedDebugGUI(const std::string& label) {
	int s = static_cast<int>(subdivision);
	ImGui::SliderInt("subdivision", &s, 4, 32);
	ImGui::DragFloat("radius", &radius.t, 0.01f);
	subdivision = static_cast<uint32_t>(s);
	label; // ラベルは使用しない
}

bool Sphere::GetChanged() {
	return radius.GetChanged() + subdivision.GetChanged();
}
