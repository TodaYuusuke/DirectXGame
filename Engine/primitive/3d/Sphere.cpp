#include "Sphere.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

Sphere::Sphere(Manager* manager) {
	primitiveManager = manager;
	material = new Material(manager);
	material->data_->enableLighting = true;
	transform.Initialize();
	transform.CreateResource(manager);

	defaultColor = new Color(WHITE);
	radius_ = 1.0f;
	CalcVertices();	// 計算
}

void Sphere::Draw(FillMode fillmode, Texture* texture) {
	transform.MatWorld();	// WorldTransformを更新
	primitiveManager->Draw(vertices, GetVertexCount(), indexes, GetIndexCount(), fillmode, &transform, material, texture, false);
}

void Sphere::Subdivision(uint32_t value) {
	subdivision_ = value;
	CalcVertices();	// 再計算
}
void Sphere::Radius(float value) {
	radius_ = value;
	CalcVertices();	// 再計算
}

void Sphere::CalcVertices() {
	// 頂点とインデックスをクリア
	vertices = new Vertex[GetVertexCount()];
	indexes = new uint32_t[GetIndexCount()];

	int arrayIndex = 0;
	
	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex <= subdivision_; ++latIndex) {
		float lat = (float)(-M_PI / 2.0f + GetLatEvery() * latIndex); // 現在の緯度
		// 緯度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex <= subdivision_; ++lonIndex) {
			float lon = lonIndex * GetLonEvery(); // 現在の経度
			// world座標系での点を求める
			vertices[arrayIndex].position =
			{ cosf(lat) * cosf(lon) * radius_, sinf(lat) * radius_, cosf(lat) * sinf(lon) * radius_ };

			// UV座標系をセット
			float u = float(lonIndex) / float(subdivision_);
			float v = 1.0f - float(latIndex) / float(subdivision_);
			vertices[arrayIndex].texCoord = { u,v };

			// 法線をセット
			vertices[arrayIndex].normal = vertices[arrayIndex].position;

			// 色をセット
			vertices[arrayIndex].color = *defaultColor;

			// 配列ずらし
			arrayIndex++;
		}
	}

	// インデックスをリセット
	arrayIndex = 0;
	// 一番下の頂点
	for (uint32_t x = 0; x < subdivision_; x++) {
		indexes[arrayIndex++] = x;
		indexes[arrayIndex++] = x + subdivision_ + 1;
		indexes[arrayIndex++] = x + subdivision_ + 2;
	}

	for (uint32_t y = 1; y <= subdivision_ - 2; y++) {
		for (uint32_t x = 0; x < subdivision_; x++) {
			indexes[arrayIndex++] = (y * (subdivision_ + 1)) + x;
			indexes[arrayIndex++] = ((y + 1) * (subdivision_ + 1)) + x;
			indexes[arrayIndex++] = (y * (subdivision_ + 1)) + (x + 1);
			indexes[arrayIndex++] = ((y + 1) * (subdivision_ + 1)) + x;
			indexes[arrayIndex++] = ((y + 1) * (subdivision_ + 1)) + (x + 1);
			indexes[arrayIndex++] = (y * (subdivision_ + 1)) + (x + 1);
		}
	}

	// 一番上の頂点
	for (uint32_t x = 0; x < subdivision_; x++) {
		indexes[arrayIndex++] = subdivision_ * subdivision_ + x - 1;
		indexes[arrayIndex++] = (subdivision_ * (subdivision_ + 1)) + x;
		indexes[arrayIndex++] = subdivision_ * subdivision_ + x;
	}
}