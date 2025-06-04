#include "Billboard2D.h"
#include "component/Resource.h"

using namespace LWP::Math;
using namespace LWP::Primitive;

Billboard2D::Billboard2D() {
	// 初期化を呼び出す
	isUI = false;
	Init();
}

void Billboard2D::DerivedDebugGUI(const std::string& label) {
	label;	// 使用しない

	ImGui::DragFloat2("billboardSize", &billboardSize.x, 1.0f);
	ImGui::DragFloat2("size", &size.x, 1.0f);
	ImGui::SliderFloat2("anchorPoint", &anchorPoint.x, 0.0f, 1.0f);
	// indexが0以上なら操作用のGUIを出す
	if (index >= 0) {
		ImGui::InputInt("index", &index);
	}
}
void Billboard2D::CreateVerticesPosition() {
	// サイズとアンカーポイントから頂点データ用の値を求める
	vertices[0].position = { billboardSize.x * -anchorPoint.x,         billboardSize.y * (1.0f - anchorPoint.y), 0.00f };	// 左上
	vertices[1].position = { billboardSize.x * (1.0f - anchorPoint.x), billboardSize.y * (1.0f - anchorPoint.y), 0.00f };	// 右上
	vertices[2].position = { billboardSize.x * (1.0f - anchorPoint.x), billboardSize.y * -anchorPoint.y,		 0.00f };	// 右下
	vertices[3].position = { billboardSize.x * -anchorPoint.x,         billboardSize.y * -anchorPoint.y,		 0.00f };	// 左下
}