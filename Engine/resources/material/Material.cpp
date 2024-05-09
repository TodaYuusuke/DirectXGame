#include "Material.h"
#include "../../base/ImGuiManager.h"

using namespace LWP::Resource;

Material::Material() {}

void Material::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		uvTransform.DebugGUI("uvTransform");
		ImGui::Checkbox("EnableLighting", &enableLighting);
		ImGui::DragFloat("shinines", &shininess);
		bool b = static_cast<bool>(fillMode);
		ImGui::Checkbox("FillMode", &b);
		fillMode = static_cast<FillMode>(b);
		ImGui::TreePop();
	}
}