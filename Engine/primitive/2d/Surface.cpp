#include "Surface.h"

using namespace LWP::Primitive;

void Surface::Create(const Math::Vector3& rightDown) {
	// 座標
	vertices[0].position = { 0.0f		,0.0f		,0.0f		 }; // 左上
	vertices[1].position = { rightDown.x,0.0f		,rightDown.z }; // 右上
	vertices[2].position = rightDown; // 右下
	vertices[3].position = { 0.0f		,rightDown.y,0.0f		 }; // 左下
}
void Surface::Create(const Math::Vector3& leftUp, const Math::Vector3& rightUp, const Math::Vector3& rightDown, const Math::Vector3& leftDown) {
	vertices[0].position = leftUp; // 左上
	vertices[1].position = rightUp; // 右上
	vertices[2].position = rightDown; // 右下
	vertices[3].position = leftDown; // 左下
}

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