#include "ResourceManager.h"

#include "math/Math.h"
#include "primitive/3d/Mesh.h"
#include "utility/ErrorReporter.h"
#include "base/DirectXCommon.h"

#include "component/Resource.h"

#include <fstream>
#include <sstream>

// assimpの読み込み
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Resource;

Manager::~Manager() {
	// XAudio2解放
	xAudio2_.Reset();
	// Media Foundation
	MFShutdown();
	// テクスチャ解放
	textureMap_.clear();
	// オーディオ解放
	audioMap_.clear();
	// モデル解放
	meshDataMap_.clear();
}

void Manager::Initialize() {
	HRESULT hr;

	// XAudio2初期化
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	// MediaFoundationの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

void Manager::Update() {
	// アニメーション更新
	for (Animation* a : animations_.list) { a->Update(); }
	// モーション更新
	for (Motion* m : motions_.list) { m->Update(); }
}

Texture Manager::LoadTexture(Base::DirectXCommon* directX, const std::string& filepath) {
	return LoadTextureLongPath(directX, textureDirectoryPath_ + filepath);
}
Texture Manager::LoadTextureLongPath(Base::DirectXCommon* directX, const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!textureMap_.count(filepath)) {
		// 新しいテクスチャをロード
		textureMap_[filepath] = TextureStruct();
		// 読み込み
		textureMap_[filepath].r.Init(directX->GetGPUDevice(), directX->GetHeaps(), filepath);
		// インデックスを保持
		textureMap_[filepath].tex = textureMap_[filepath].r;
	}
	return textureMap_[filepath].tex;
}

AudioData* Manager::LoadAudio(const std::string& filepath) {
	return LoadAudioLongPath(audioDirectoryPath_ + filepath);
}
AudioData* Manager::LoadAudioLongPath(const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!audioMap_.count(filepath)) {
		// 新しいテクスチャをロード
		audioMap_[filepath] = AudioData(xAudio2_.Get(), filepath);
	}
	return &audioMap_[filepath];
}

const Primitive::MeshData& Manager::LoadMesh(const std::string& filepath) {
	return LoadMeshLongPath(meshDirectoryPath_ + filepath);
}
const Primitive::MeshData& Manager::LoadMeshLongPath(const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!meshDataMap_.count(filepath)) {
		// 新しいテクスチャをロード
		//meshDataMap_[filepath];	// 要素は自動追加されるらしい
		// 現在はobjのみ読み込み可
		//LoadObj(&meshDataMap_[filepath], filepath);
		meshDataMap_[filepath] = LoadAssimp(filepath);
	}
	return meshDataMap_[filepath];
}

Primitive::MeshData Manager::LoadAssimp(const std::string& filepath) {
	Primitive::MeshData result{};	// 結果

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
			if(hasTexcoord) {
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