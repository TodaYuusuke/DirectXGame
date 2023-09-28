#pragma once
#include "IPrimitive.h"
#include "PrimitiveManager.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

IPrimitive::IPrimitive(Manager* manager) {
	// マネージャーのポインタを受け取る
	primitiveManager = manager;
	
	// マテリアル作成
	material = new Material(manager);
	material->data_->enableLighting = false;

	// トランスフォーム初期化
	transform.Initialize();
	transform.CreateResource(manager);
}

void IPrimitive::InitializeVertices() {
	// 頂点のサイズ作成
	vertices = new Vertex[GetVertexCount()];
}

void IPrimitive::Draw(Resource::Texture* texture) {
	// テクスチャがnullじゃなければuvTransformを受け取る
	if (texture != nullptr) {
		material->data_->uvMatrix = texture->GetMatrix();
	}

	transform.MatWorld();	// WorldTransformを更新
	primitiveManager->Draw(vertices, GetVertexCount(), indexes, GetIndexCount(), commonColor, &transform, material, texture, fillMode, isUI);
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
		material->DebugGUI();

		// 共通カラーがあるとき -> ColorEdit4を呼び出す
		if (commonColor != nullptr) {
			LWP::Base::ImGuiManager::ColorEdit4("commonColor", *commonColor);
			if (ImGui::Button("Delete CommonColor")) {
				delete commonColor;
				commonColor = nullptr;
			}
		}
		// 共通カラーがないとき -> 共通カラーを作る
		else if (ImGui::Button("Create CommonColor")){
			commonColor = new Utility::Color(Utility::ColorPattern::WHITE);
		}

		ImGui::Checkbox("Fill", (bool*)&fillMode);
		ImGui::Checkbox("isUI", &isUI);
		DerivedDebugGUI();
		ImGui::TreePop();
	}
}

void IPrimitive::DerivedDebugGUI(const std::string& label) {
	// 基底クラスでは記述無し
	label;
}


int IPrimitive::GetVertexCount() const {
	return 1;
}
int IPrimitive::GetIndexCount() const {
	return 0;
}