#include "Material.h"
#include "base/ImGuiManager.h"

#include "component/Resource.h"
#include <string>

#include "Config.h"

using namespace LWP::Base;
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

	// テクスチャを取得
	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
		aiString textureFilePath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

		texture = LWP::Resource::LoadTextureLongPath(Utility::ConvertToParentDirectory(filePath) + textureFilePath.C_Str());
	}

	// マテリアルカラーを取得
	aiColor4D diffuse;
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
		color = Utility::Color(float(diffuse.r), float(diffuse.g), float(diffuse.b), float(diffuse.a));
	}
}

void Material::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		uvTransform.DebugGUI("uvTransform");
		LWP::Base::ImGuiManager::ColorEdit4("color", color);
		ImGui::DragFloat("shinines", &shininess);
		ImGui::Checkbox("enableLighting", &enableLighting);
		if (ImGui::TreeNode("Texture")) {
			ImGuiManager::ShowTexture(texture.t);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

MaterialStruct& MaterialStruct::operator=(const Primitive::Material& value) {
	uvMatrix = value.uvTransform.GetAffineMatrix();
	color = value.color.GetVector4();
	shininess = value.shininess;
	enableLighting = value.enableLighting;
	// テクスチャのインデックスを貰う
	textureIndex = value.texture.t.GetIndex() != -1 ?
		value.texture.t.GetIndex() :
		lwpC::Rendering::kMaxBuffer;	// テクスチャの配列の一番最初のインデックス
	// SRV上のオフセット分戻して考える
	textureIndex -= lwpC::Rendering::kMaxBuffer;

	return *this;
}
