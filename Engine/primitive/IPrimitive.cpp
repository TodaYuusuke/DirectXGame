#include "IPrimitive.h"
#include "base/directX/RendererManager.h"
#include "base/ImGuiManager.h"
#include "component/Primitive.h"

namespace LWP::Primitive {
	void IPrimitive::Init() {
		worldTF.Init();
		//material.Init();
	}

	void IPrimitive::ShowNameGUI() {
		// 固有名詞変更
		if (ImGui::InputText("Name", &name)) {
			// 名前無しは禁止
			if (name.empty()) { name = "noName"; }
		}
	}
	void IPrimitive::ShowVertexGUI(Vertex* v, int size) {
		// 頂点の配列を参照
		if (ImGui::TreeNode("Vertex")) {
			static int vertexNum_ = 0;
			ImGui::SliderInt("vertexNumber", &vertexNum_, 0, size - 1);
			ImGui::DragFloat3("position", &v[vertexNum_].position.x, 0.01f);
			ImGui::DragFloat2("texCoord", &v[vertexNum_].texCoord.x, 0.01f);
			ImGui::DragFloat3("normal", &v[vertexNum_].normal.x, 0.01f);
			LWP::Base::ImGuiManager::ColorEdit4("color", v[vertexNum_].color);
			ImGui::TreePop();
		}
	}
	void IPrimitive::ShowTransformGUI() { worldTF.DebugGUI(); }
	void IPrimitive::ShowMaterialGUI() { material.DebugGUI(); }
	void IPrimitive::ShowIsActiveGUI() { ImGui::Checkbox("isActive", &isActive); }
	void IPrimitive::CallInitGUI() { if (ImGui::Button("Call Init")) { Init(); } }
}