#include "Sprite.h"

#include "primitive/PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include <algorithm>

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

Sprite::Sprite() {
	// 管理クラスからRendererManagerのポインタを受け取る
	manager_ = Primitive::Manager::GetInstance()->GetRendererManager();
}
Sprite::~Sprite() {}

void Sprite::Draw(Vector2 pos, float rotateZ, Vector2 scale, Color color) {
	pos; rotateZ; scale; color;
}
void Sprite::DrawBillboard2D(Vector3 pos, Vector3 rotate, Vector2 scale, Color color) {
	pos; rotate; scale; color;

}
void Sprite::DrawBillboard3D(Vector3 pos, Vector3 rotate, Vector2 scale, Color color) {
	pos; rotate; scale; color;
}

void Sprite::DebugGUI() {
	if (ImGui::TreeNode(name.c_str())) {
		if (ImGui::TreeNode("Texture")) {
			ImGuiManager::ShowTexture(texture);
			ImGui::TreePop();
		}
		ImGui::SliderFloat2("anchorPoint", &anchorPoint.x, 0.0f, 1.0f);
	}
}
//
//void Sprite::Update() {
//	// 値が変化した場合にのみ再計算
//	if (!GetChanged()) { return; }
//	// テクスチャのサイズを取得
//	if (material.texture.t.GetIndex() != -1) {
//		size.t = material.texture.t.GetSize();
//	}
//
//	// 頂点を再計算
//	CreateVertices();
//}
//
//void Sprite::CreateVertices() {
//	// 頂点の作成
//	vertices.clear();
//	vertices.resize(GetVertexCount());
//	
//	// サイズとアンカーポイントから頂点データ用の値を求める
//
//	// 座標
//	vertices[0].position = { size.t.x * -anchorPoint.t.x,         size.t.y * -anchorPoint.t.y,         0.00f };	// 左上
//	vertices[1].position = { size.t.x * (1.0f - anchorPoint.t.x), size.t.y * -anchorPoint.t.y,         0.00f };	// 右上
//	vertices[2].position = { size.t.x * (1.0f - anchorPoint.t.x), size.t.y * (1.0f - anchorPoint.t.y), 0.00f };	// 右下
//	vertices[3].position = { size.t.x * -anchorPoint.t.x,         size.t.y * (1.0f - anchorPoint.t.y), 0.00f };	// 左下
//	// テクスチャ座標
//	vertices[0].texCoord = { 0.0f,0.0f };
//	vertices[1].texCoord = { 1.0f,0.0f };
//	vertices[2].texCoord = { 1.0f,1.0f };
//	vertices[3].texCoord = { 0.0f,1.0f };
//	// 法線
//	for (int i = 0; i < GetVertexCount(); i++) {
//		vertices[i].normal = { 0.0f,0.0f,-1.0f };
//	}
//};
//
//void Sprite::DerivedDebugGUI(const std::string& label) {
//	label;	// 使用しない
//
//	ImGui::DragFloat2("size", &size.t.x, 1.0f);
//	ImGui::SliderFloat2("anchorPoint", &anchorPoint.t.x, 0.0f, 1.0f);
//}
//
//bool Sprite::GetChanged() {
//	return material.texture.GetChanged() + anchorPoint.GetChanged();
//}
