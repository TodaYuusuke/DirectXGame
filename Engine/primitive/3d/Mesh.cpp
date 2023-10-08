#include "Mesh.h"

#include "../../../Adapter/Adapter.h"
#include "../../utility/ErrorReporter.h"

#include <fstream>
#include <sstream>
#include <map>

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

void Mesh::LoadFile(const std::string& filename) {
	// 頂点とインデックスは先にclearしておく
	vertices.clear();
	indexes.clear();

	// 現在はobjのみ読み込み可
	LoadObj(filename);
}

void Mesh::LoadObj(const std::string& filename) {
	// インデックス登録用の3次元配列
	std::map<int, std::map<int, std::map<int, int>>> key;

	std::vector<Vector3> positions;	// 位置
	std::vector<Vector2> texcoords;	// 法線
	std::vector<Vector3> normals;	// テクスチャ座標

	std::ifstream file(directoryPath_ + filename);
	LoadAssert(file.is_open(), filename);
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
				if (value > 0 && value < vertices.size()) {
					indexes.push_back(static_cast<uint32_t>(value - 1)); // パターンが見つかった場合のインデックス
				}
				// 新しいパターンの場合 -> 新しく頂点を追加し、インデックスに追加
				else {
					// 新しい頂点を生成
					Vertex newVertex;
					newVertex.position = positions[std::stoi(elements[0]) - 1];
					newVertex.texCoord = texcoords[std::stoi(elements[1]) - 1];
					newVertex.normal = normals[std::stoi(elements[2]) - 1];
					// 左手座標系なので座標を変換
					newVertex.position.x *= -1.0f;
					newVertex.texCoord.y = 1.0f - newVertex.texCoord.y;
					newVertex.normal.x *= -1.0f;
					// 頂点追加
					vertices.push_back(newVertex);
					// インデックスを登録
					indexes.push_back(static_cast<uint32_t>(vertices.size() - 1));

					// 既存のパターンであることを指すために追加
					key[std::stoi(elements[0])][std::stoi(elements[1])][std::stoi(elements[2])] = static_cast<int>(vertices.size());
				}
			}
		}
		// マテリアル読み込み
		else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			// 必ずobjと同一階層にmtlは存在させる
			LoadMtl(materialFilename);
		}
	}

	// 最後にインデックスを反転させる（左手座標系に修正するため）
	for (size_t i = 0; i < indexes.size(); i += 3) {
		// 0番目と2番目の要素を入れ替える
		std::swap(indexes[i], indexes[i + 2]);
	}
}

void Mesh::LoadMtl(const std::string& filename) {
	std::ifstream file(directoryPath_ + filename);
	LoadAssert(file.is_open(), filename);
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
			texture = LWP::Resource::LoadTextureLongPath(directoryPath_ + textureFilename);
		}
	}
}