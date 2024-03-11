#include "Capsule.h"
#include "../../../Adapter/LWP.h"

#include "object/collider/cSphere.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

void Capsule::CreateVertices() {
	// 頂点をクリア
	vertices.clear();
	vertices.resize(GetVertexCount());

	int arrayIndex = 0;
	
	// カプセルの長さ
	float length = (end.t - start()).Length();
	// カプセルの回転行列
	Matrix4x4 rotate = Matrix4x4::DirectionToDirection(
		{0.0f,1.0f,0.0f}, (end.t - start()).Normalize()
	);


	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex <= subdivision; ++latIndex) {
		float lat = (float)(-M_PI / 2.0f + GetLatEvery() * latIndex); // 現在の緯度
		// 緯度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex <= subdivision; ++lonIndex) {
			float lon = lonIndex * GetLonEvery(); // 現在の経度
			// world座標系での点を求める
			vertices[arrayIndex].position =
			{ cosf(lat) * cosf(lon) * radius, sinf(lat) * radius, cosf(lat) * sinf(lon) * radius };

			// 後半の処理 -> 頂点は距離分ずらす
			if (latIndex > subdivision / 2u) {
				vertices[arrayIndex].position.y += length;
			}
			// 回転！
			vertices[arrayIndex].position = vertices[arrayIndex].position * rotate;

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

void Capsule::CreateIndexes() {
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



int Capsule::GetVertexCount() const { return (subdivision + 1u) * (subdivision + 1u); }
int Capsule::GetIndexCount() const { return subdivision * (subdivision - 1u) * 2u * 3u; }

void Capsule::DerivedDebugGUI(const std::string& label) {
	ImGui::DragFloat3("start", &transform.translation.x, 0.01f);
	ImGui::DragFloat3("end", &end.t.x, 0.01f);
	int s = static_cast<int>(subdivision);
	ImGui::SliderInt("subdivision", &s, 4, 32);
	subdivision = static_cast<uint32_t>(s);
	ImGui::DragFloat("radius", &radius.t, 0.01f);
	label; // ラベルは使用しない
}

bool Capsule::GetChanged() {
	return radius.GetChanged() + subdivision.GetChanged() + obsTransform.GetChanged() + end.GetChanged();
}
