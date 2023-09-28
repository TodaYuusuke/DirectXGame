#include "Material.h"
#include "../primitive/PrimitiveManager.h"
#include "../object/WorldTransform.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Resource;

Material::Material(Primitive::Manager* primitiveManager) {
	data_ = new MaterialStruct();
	resource_ = primitiveManager->CreateBufferResource(sizeof(MaterialStruct));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// 単位行列を入れる
	data_->uvMatrix = Math::Matrix4x4::CreateIdentity4x4();
	// ワールドトランスフォームを初期化
	uvTransform.Initialize();
}

D3D12_GPU_VIRTUAL_ADDRESS Material::GetGPUVirtualAddress() {
	// アドレスを返す前にuvTransformを更新
	data_->uvMatrix = uvTransform.MatWorld();
	return resource_.Get()->GetGPUVirtualAddress();
}

void Material::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		uvTransform.DebugGUI("uvTransform");
		ImGui::Checkbox("enableLighting", (bool*)&data_->enableLighting);
		ImGui::TreePop();
	}
}