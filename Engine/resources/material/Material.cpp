#include "Material.h"
#include "../../base/CommandManager.h"
#include "../../base/ImGuiManager.h"

using namespace LWP::Resource;

Material::Material(Base::CommandManager* manager) {
	// リソースを作成
	index_ = manager->CreateMaterialResource();

	// ワールドトランスフォームを初期化
	uvTransform.Initialize();
}

void Material::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		uvTransform.DebugGUI("uvTransform");
		ImGui::Checkbox("EnableLighting", &enableLighting);
		ImGui::Checkbox("FillMode", (bool*)fillMode);
		ImGui::TreePop();
	}
}