#include "ModelData.h"

#include <cassert>


using namespace LWP::Primitive;
using namespace LWP::Resource;

/// <summary>
/// 3Dモデルのデータを読み込む
/// </summary>
/// <param name="filename">読み込むファイルの名前</param>
void ModelData::Load(const std::string& filePath) {
	Assimp::Importer importer;
	// ファイル読み込み（三角形の並び順を逆にする | UVをフリップする | 四角形以上のポリゴンを三角形に自動分割）
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
	assert(scene->HasMeshes());	// メッシュがないのは対応しない

	// 埋め込まれた画像があるかを検出
	//for (uint32_t texIndex = 0; meshIndex < scene->tex; meshIndex++) {
	//	if (scene->GetEmbeddedTexture()) {

	//}


	// Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		// 新しいメッシュを生成
		meshes_.emplace_back();
		// 読み込む
		meshes_.back().Load(scene->mMeshes[meshIndex]);
	}

	// ノード情報を格納
	//if (scene->mRootNode) {
	//	result.node.ReadNode(scene->mRootNode);
	//}

	// マテリアルの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		// 新しいメッシュを生成
		material_.emplace_back();
		// 読み込む
		material_.back().Load(scene->mMaterials[materialIndex], scene, filePath);
	}
}