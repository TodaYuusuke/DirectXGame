#include "ModelData.h"

// assimpの読み込み
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace LWP::Primitive;

/// <summary>
/// 初期化
/// </summary>
void ModelData::Init() {

}

/// <summary>
/// 3Dモデルのデータを読み込む
/// </summary>
/// <param name="filename">読み込むファイルの名前</param>
void ModelData::Load(const std::string& filename) {
	Assimp::Importer importer;
	// ファイル読み込み（三角形の並び順を逆にする | UVをフリップする | 四角形以上のポリゴンを三角形に自動分割）
	const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
	assert(scene->HasMeshes());	// メッシュがないのは対応しない

	// Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		bool hasNormal = mesh->HasNormals();
		bool hasTexcoord = mesh->HasTextureCoords(0);

		// Vertexの解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
			Primitive::Vertex newVertex;

			// インデックス情報を元に情報を取得する
			aiVector3D& position = mesh->mVertices[vertexIndex];		 // 頂点座標取得
			newVertex.position = { -position.x, position.y, position.z }; // 頂点座標追加

			// uv座標チェック
			if (hasTexcoord) {
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex]; // テクスチャ座標取得
				newVertex.texCoord = { texcoord.x, texcoord.y };			  // テクスチャ座標追加
			}
			else {
				newVertex.texCoord = { 0.0f,0.0f };
			}

			// 法線チェック
			if (hasNormal) {
				aiVector3D& normal = mesh->mNormals[vertexIndex];			 // 法線取得
				newVertex.normal = { -normal.x, normal.y, normal.z };		  // 法線追加
			}
			else {
				newVertex.normal = newVertex.position;
			}

			// 頂点データを追加
			result.vertices.push_back(newVertex);
		}

		// Face(Meshの中身)の解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);	// 三角形のみサポート

			// インデックス解析
			for (uint32_t element = 0; element < face.mNumIndices; element++) {
				uint32_t vertexIndex = face.mIndices[element];
				// インデックス情報を追加
				result.indexes.push_back(vertexIndex);
			}
		}

		// マテリアルの解析
		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
			aiMaterial* material = scene->mMaterials[materialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				result.texture = LWP::Resource::LoadTextureLongPath(Utility::ConvertToParentDirectory(filepath) + textureFilePath.C_Str());
			}
		}

		// ノード情報を格納
		if (scene->mRootNode) {
			result.node.ReadNode(scene->mRootNode);
		}
	}

	return result;
}