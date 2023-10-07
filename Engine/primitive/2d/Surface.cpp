#include "Surface.h"

using namespace LWP::Primitive;

void Surface::CreateVertices() {
	// 頂点の作成
	vertices.clear();
	vertices.resize(GetVertexCount());
	
	// 座標
	vertices[0].position = { -0.35f, 0.35f,0.00f };	// 左上
	vertices[1].position = {  0.35f, 0.35f,0.00f };	// 右上
	vertices[2].position = {  0.35f,-0.35f,0.00f };	// 右下
	vertices[3].position = { -0.35f,-0.35f,0.00f };	// 左下
	// テクスチャ座標
	vertices[0].texCoord = { 0.0f,0.0f };
	vertices[1].texCoord = { 1.0f,0.0f };
	vertices[2].texCoord = { 1.0f,1.0f };
	vertices[3].texCoord = { 0.0f,1.0f };
	// 法線
	for (int i = 0; i < GetVertexCount(); i++) {
		vertices[i].normal = { 0.0f,0.0f,-1.0f };
	}
};