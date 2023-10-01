#include "Triangle.h"

using namespace LWP::Primitive;

void Triangle::CreateVertices() {
	// 頂点の作成
	vertices.clear();
	vertices.resize(GetVertexCount());
	indexes.clear();
	indexes.resize(GetIndexCount());
	
	// 座標
	vertices[0].position = {  0.00f, 0.50f,0.00f };	// 上
	vertices[1].position = {  0.43f,-0.25f,0.00f };	// 右下
	vertices[2].position = { -0.43f,-0.25f,0.00f };	// 左下
	// テクスチャ座標
	vertices[0].texCoord = { 0.5f,0.0f };
	vertices[1].texCoord = { 1.0f,1.0f };
	vertices[2].texCoord = { 0.0f,1.0f };
	// 法線
	for (int i = 0; i < GetVertexCount(); i++) {
		vertices[i].normal = { 0.0f,0.0f,-1.0f };
	}
};