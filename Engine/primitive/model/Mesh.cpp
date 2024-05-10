#include "Mesh.h"

#include "component/Resource.h"

using namespace LWP::Primitive;
using namespace LWP::Math;


void Mesh::Load(aiMesh* mesh) {
	// 名前を取得
	name = mesh->mName.C_Str();

	// texcoordを持っているか取得
	bool hasTexcoord = mesh->HasTextureCoords(0);
	// 法線を持っているか取得
	bool hasNormal = mesh->HasNormals();

	// Vertexの解析
	for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
		Vertex newVertex;

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
		vertices.push_back(newVertex);
	}

	// Face(Meshの中身)の解析
	for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
		aiFace& face = mesh->mFaces[faceIndex];
		assert(face.mNumIndices == 3);	// 三角形のみサポート

		// インデックス解析
		for (uint32_t element = 0; element < face.mNumIndices; element++) {
			uint32_t vertexIndex = face.mIndices[element];
			// インデックス情報を追加
			indexes.push_back(vertexIndex);
		}
	}

	// マテリアルインデックス取得
	materialIndex = mesh->mMaterialIndex;
}