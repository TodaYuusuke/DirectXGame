#include "Sprite.h"

#include "component/Resource.h"
#include "base/ImGuiManager.h"

#include <algorithm>

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Primitive;

void Sprite::LoadTexture(const std::string& fileName) {
	material.texture = Resource::LoadTexture(fileName);
	size = material.texture.t.GetSize();	// テクスチャのサイズを取得
}
void Sprite::SetSplitSize(Vector2 splitSize) {
	size = splitSize;	// サイズを分割サイズにセット
	index = 0;	// インデックスを0にセット
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

	// 分割インデックスが0以下の場合
	if (index < 0) {
		// デフォルトのテクスチャ座標
		vertices[0].texCoord = { 0.0f,0.0f };
		vertices[1].texCoord = { 1.0f,0.0f };
		vertices[2].texCoord = { 1.0f,1.0f };
		vertices[3].texCoord = { 0.0f,1.0f };
	}
	else {
		// 分割されたUV座標を計算
		Vector2 textureSize = material.texture.t.GetSize();	// テクスチャのサイズを取得
		float splitUVx = size.x / textureSize.x;
		float splitUVy = size.y / textureSize.y;

		// 分割画像上のXY座標を求める
		float x = (index * splitUVx) - static_cast<int>(index * splitUVx);
		float y = splitUVy * static_cast<int>(index * splitUVx);

		// デフォルトのテクスチャ座標
		vertices[0].texCoord = { x,            y };
		vertices[1].texCoord = { x + splitUVx, y };
		vertices[2].texCoord = { x + splitUVx, y + splitUVy };
		vertices[3].texCoord = { x,            y + splitUVy };
	}


	// 法線
	for (int i = 0; i < GetVertexCount(); i++) {
		vertices[i].normal = { 0.0f,0.0f,-1.0f };
	}
};

void Sprite::DerivedDebugGUI(const std::string& label) {
	label;	// 使用しない

	ImGui::DragFloat2("size", &size.x, 1.0f);
	ImGui::SliderFloat2("anchorPoint", &anchorPoint.x, 0.0f, 1.0f);
	// indexが0以上なら操作用のGUIを出す
	if (index >= 0) {
		ImGui::InputInt("index", &index);
	}
}

bool Sprite::GetChanged() {
	return material.texture.GetChanged();
}
