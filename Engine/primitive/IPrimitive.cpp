#pragma once
#include "IPrimitive.h"
#include "../base/directX/command/CommandManager.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

IPrimitive::IPrimitive(Base::CommandManager* manager)
	: material(manager) 
{
	// トランスフォーム初期化
	transform.Initialize();
}

void IPrimitive::CreateVertices() {
	// 頂点のサイズ作成
	vertices.clear();
	vertices.resize(GetVertexCount());
}
void IPrimitive::CreateIndexes() {
	// 頂点のサイズ作成
	indexes.clear();
	indexes.resize(GetIndexCount());

	// 1ループで三角形1つ分のインデックス生成
	for (int i = 0; i < GetVertexCount() - 2; i++) {
		indexes[i * 3] = 0;
		indexes[i * 3 + 1] = i + 1;
		indexes[i * 3 + 2] = i + 2;
	}
}

void IPrimitive::Draw(Base::CommandManager* manager) {
	// アクティブでなければ描画しない
	if (!isActive) { return; }

	manager->Draw(this);
}

void IPrimitive::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		// 頂点
		if (ImGui::TreeNode("Vertex")) {
			ImGui::SliderInt("vertexNumber", &vertexNum_, 0, GetVertexCount() - 1);
			ImGui::DragFloat3("position", &vertices[vertexNum_].position.x, 0.01f);
			ImGui::DragFloat2("texCoord", &vertices[vertexNum_].texCoord.x, 0.01f);
			ImGui::DragFloat3("normal", &vertices[vertexNum_].normal.x, 0.01f);
			LWP::Base::ImGuiManager::ColorEdit4("color", vertices[vertexNum_].color);
			ImGui::TreePop();
		}
		transform.DebugGUI();
		material.DebugGUI();

		// 共通カラーがあるとき -> ColorEdit4を呼び出す
		if (ImGui::TreeNode("CommonColor")) {
			if (commonColor != nullptr) {
				LWP::Base::ImGuiManager::ColorEdit4("color", *commonColor);
				if (ImGui::Button("Delete CommonColor")) {
					delete commonColor;
					commonColor = nullptr;
				}
			}
			// 共通カラーがないとき -> 共通カラーを作る
			else if (ImGui::Button("Create CommonColor")) {
				commonColor = new Utility::Color(Utility::ColorPattern::WHITE);
			}
			ImGui::TreePop();
		}

		// その他
		if (ImGui::TreeNode("Other")) {
			ImGui::Checkbox("isUI", &isUI);
			ImGui::Checkbox("isActive", &isActive);
			DerivedDebugGUI();
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void IPrimitive::DerivedDebugGUI(const std::string& label) {
	// 基底クラスでは記述無し
	label;
}


int IPrimitive::GetVertexCount() const {
	return 3;
}
int IPrimitive::GetIndexCount() const {
	return (GetVertexCount() - 2) * 3;	// インデックスの数を求める
}