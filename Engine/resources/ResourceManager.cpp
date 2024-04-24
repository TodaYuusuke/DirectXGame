#include "ResourceManager.h"

#include "math/Math.h"
#include "primitive/3d/Mesh.h"
#include "utility/ErrorReporter.h"
#include "base/DirectXCommon.h"

#include "component/Resource.h"

#include <fstream>
#include <sstream>

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
	// モーション更新
	for (Motion* m : motions_.list) {
		m->Update();
	}
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
		meshDataMap_[filepath];	// 要素は自動追加されるらしい
		// 現在はobjのみ読み込み可
		LoadObj(&meshDataMap_[filepath], filepath);
	}
	return meshDataMap_[filepath];
}


// 3Dモデルの形式別読み込み処理
void Manager::LoadObj(Primitive::MeshData* mesh, const std::string& filepath) {
	// インデックス登録用の3次元配列
	std::map<int, std::map<int, std::map<int, int>>> key;

	std::vector<Vector3> positions;	// 位置
	std::vector<Vector2> texcoords;	// 法線
	std::vector<Vector3> normals;	// テクスチャ座標

	std::ifstream file(filepath);
	LoadAssert(file.is_open(), filepath);
	std::string line;	// ファイルから読み込んだ1行

	// ファイルを一行ずつ読み込む
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	// 先頭の識別子を読む

		// identifierに応じた処理

		// 頂点データ登録
		if (identifier == "v") {
			Vector3 position;
			s >> position.x >> position.y >> position.z;
			positions.push_back(position);
		}
		// テクスチャ座標登録
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		// 法線登録
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		// 頂点とインデックス登録
		else if (identifier == "f") {
			// 面は三角形限定。その他は未対応
			// 3頂点ループ
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);	// 1頂点分のデータ
				std::string elements[3];	// 0 → 位置 : 1 → UV : 2 → 法線
				for (int32_t i = 0; i < 3; ++i) {
					std::getline(v, elements[i], '/');
				}
				// 次の処理に移る前に、これが新しいパターンの頂点かチェック
				int value = key[std::stoi(elements[0])][std::stoi(elements[1])][std::stoi(elements[2])];
				// 既存のパターンだった場合 -> 既存の頂点のインデックスを求めてからインデックスに追加
				if (value > 0 && value < mesh->vertices.size()) {
					mesh->indexes.push_back(static_cast<uint32_t>(value - 1)); // パターンが見つかった場合のインデックス
				}
				// 新しいパターンの場合 -> 新しく頂点を追加し、インデックスに追加
				else {
					// 新しい頂点を生成
					Primitive::Vertex newVertex;
					newVertex.position = positions[std::stoi(elements[0]) - 1];
					newVertex.texCoord = texcoords[std::stoi(elements[1]) - 1];
					newVertex.normal = normals[std::stoi(elements[2]) - 1];
					// 左手座標系なので座標を変換
					newVertex.position.x *= -1.0f;
					newVertex.texCoord.y = 1.0f - newVertex.texCoord.y;
					newVertex.normal.x *= -1.0f;
					// 頂点追加
					mesh->vertices.push_back(newVertex);
					// インデックスを登録
					mesh->indexes.push_back(static_cast<uint32_t>(mesh->vertices.size() - 1));

					// 既存のパターンであることを指すために追加
					key[std::stoi(elements[0])][std::stoi(elements[1])][std::stoi(elements[2])] = static_cast<int>(mesh->vertices.size());
				}
			}
		}
		// マテリアル読み込み
		else if (identifier == "mtllib") {
			std::string materialFilepath;
			s >> materialFilepath;
			// 必ずobjと同一階層にmtlは存在させる
			LoadMtl(mesh, Utility::ConvertToParentDirectory(filepath) + materialFilepath);
		}
	}

	// 最後にインデックスを反転させる（左手座標系に修正するため）
	for (size_t i = 0; i < mesh->indexes.size(); i += 3) {
		// 0番目と2番目の要素を入れ替える
		std::swap(mesh->indexes[i], mesh->indexes[i + 2]);
	}

}


// マテリアルの読み込み
void Manager::LoadMtl(Primitive::MeshData* mesh, const std::string& filepath) {
	std::ifstream file(filepath);
	LoadAssert(file.is_open(), filepath);
	std::string line;	// ファイルから読み込んだ1行

	// ファイルを一行ずつ読み込む
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	// 先頭の識別子を読む

		// identifierに応じた処理

		// テクスチャ読み込み
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// テクスチャを読み込む
			mesh->texture = LWP::Resource::LoadTextureLongPath(Utility::ConvertToParentDirectory(filepath) + textureFilename);
		}
	}
}