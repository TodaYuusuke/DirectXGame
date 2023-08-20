#include "Sphere.h"
#include "../2d/Surface.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

Sphere::Sphere(Manager* manager) {
	primitiveManager = manager;
	transform.Initialize();
	for (uint32_t y = 0; y < kSubdivision; y++) {
		for (uint32_t x = 0; x < kSubdivision; x++) {
			surfaces[y][x] = LWP::Engine::CreatePrimitiveInstance<Surface>();
		}
	}
	defaultColor = new Color(WHITE);

	center = { 0.0f,0.0f,0.0f };
	radius = 1.0f;
}

void Sphere::Draw(FillMode fillmode, Texture* texture) {
	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = (float)(-M_PI / 2.0f + kLatEvery * latIndex); // 現在の緯度
		// 緯度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度
			// world座標系でのa,b,c,dを求める
			Vector3 a, b, c, d;
			a = { cosf(lat) * cosf(lon) * radius, sinf(lat) * radius, cosf(lat) * sinf(lon) * radius };
			a += center;
			b = { cosf(lat + kLatEvery) * cosf(lon) * radius, sinf(lat + kLatEvery) * radius, cosf(lat + kLatEvery) * sinf(lon) * radius };
			b += center;
			c = { cosf(lat) * cosf(lon + kLonEvery) * radius, sinf(lat) * radius, cosf(lat) * sinf(lon + kLonEvery) * radius };
			c += center;
			d = { cosf(lat + kLatEvery) * cosf(lon + kLonEvery) * radius, sinf(lat + kLatEvery) * radius, cosf(lat + kLatEvery) * sinf(lon + kLonEvery) * radius };
			d += center;
			
			// 平面の頂点にセット
			surfaces[latIndex][lonIndex]->vertices[0].position = a;
			surfaces[latIndex][lonIndex]->vertices[1].position = b;
			surfaces[latIndex][lonIndex]->vertices[2].position = d;
			surfaces[latIndex][lonIndex]->vertices[3].position = c;


			// UV座標系も求める
			float u = float(lonIndex) / float(kSubdivision);
			float v = 1.0f - float(latIndex) / float(kSubdivision);
			Vector2 uvA, uvB, uvC, uvD;

			uvA = { u,v + (1.0f / kSubdivision) };
			uvB = { u,v };
			uvC = { u + (1.0f / kSubdivision),v + (1.0f / kSubdivision) };
			uvD = { u + (1.0f / kSubdivision),v };

			// 平面のUV座標をセット
			surfaces[latIndex][lonIndex]->vertices[0].texCoord = uvA;
			surfaces[latIndex][lonIndex]->vertices[1].texCoord = uvB;
			surfaces[latIndex][lonIndex]->vertices[2].texCoord = uvD;
			surfaces[latIndex][lonIndex]->vertices[3].texCoord = uvC;
			// 色をセット
			surfaces[latIndex][lonIndex]->defaultColor = defaultColor;
			// ワールドトランスフォームをセット
			surfaces[latIndex][lonIndex]->transform = transform;
		}
	}


	for (uint32_t y = 0; y < kSubdivision; y++) {
		for (uint32_t x = 0; x < kSubdivision; x++) {
			surfaces[y][x]->Draw(fillmode, texture);
		}
	}
}