#include "Material.h"
#include "base/ImGuiManager.h"

#include "component/Resource.h"
#include "utility/MyUtility.h"
#include <string>

using namespace LWP::Primitive;


void Material::Load(aiMaterial* material, const aiScene* scene, const std::string& filePath) {
	// 埋め込まれている場合
	aiString texture_file;
	material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
	if (const aiTexture* t = scene->GetEmbeddedTexture(texture_file.C_Str())) {
		// テクスチャがファイルに埋め込まれているので処理をやめる
		return;
	}
	// 埋め込まれていないので通常通り処理を続行

	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
		aiString textureFilePath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

		texture = LWP::Resource::LoadTextureLongPath(Utility::ConvertToParentDirectory(filePath) + textureFilePath.C_Str());
	}
}

void Material::DebugGUI() {
	if (ImGui::TreeNode("Material")) {
		uvTransform.DebugGUI("uvTransform");
		ImGui::DragFloat("shinines", &shininess);
		/* 文字列を変更する処理
		if (ImGui::InputText("Name", &name)) {
			// 名前無しは禁止
			if (name.empty()) { name = "noName"; }
		}*/
		ImGui::TreePop();
	}
}