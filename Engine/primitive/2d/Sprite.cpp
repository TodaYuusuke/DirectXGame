#include "Sprite.h"

#include "component/Resource.h"
#include "base/ImGuiManager.h"

#include <algorithm>

using namespace LWP;
using namespace LWP::Primitive;

void Sprite::LoadTexture(const std::string& fileName) {
	material.texture = Resource::LoadTexture(fileName);
	size = material.texture.t.GetSize();	// テクスチャのサイズを取得
}

void Sprite::Update() {
	// 値が変化した場合にのみ再計算
	//if (!GetChanged()) { return; }

	// 頂点を再計算
	CreateVertices();
}

void Sprite::CreateVertices() {
	// 頂点の作成
	vertices.clear();
	vertices.resize(GetVertexCount());
	
	// サイズとアンカーポイントから頂点データ用の値を求める

	// 座標
	vertices[0].position = { size.x * -anchorPoint.x,         size.y * -anchorPoint.y,         0.00f };	// 左上
	vertices[1].position = { size.x * (1.0f - anchorPoint.x), size.y * -anchorPoint.y,         0.00f };	// 右上
	vertices[2].position = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y), 0.00f };	// 右下
	vertices[3].position = { size.x * -anchorPoint.x,         size.y * (1.0f - anchorPoint.y), 0.00f };	// 左下
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

void Sprite::DerivedDebugGUI(const std::string& label) {
	label;	// 使用しない

	ImGui::DragFloat2("size", &size.x, 1.0f);
	ImGui::SliderFloat2("anchorPoint", &anchorPoint.x, 0.0f, 1.0f);
}

bool Sprite::GetChanged() {
	return material.texture.GetChanged();
}
