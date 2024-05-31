#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace LWP::Primitive {
	/// <summary>
	/// 3Dモデルのデータ構造体
	/// </summary>
	struct OldMeshData {
		// 描画する頂点
		std::vector<Vertex> vertices;
		// インデックス
		std::vector<uint32_t> indexes;
		// マテリアル
		Primitive::Material material;
		// テクスチャ
		Resource::Texture texture;

		// ノード情報
		Node node;
	};

	/// <summary>
	/// 3Dモデル
	/// </summary>
	class OldMesh final
		: public IPrimitive {
	public: // ** メンバ関数 ** //

		// 初期化を呼び出す
		OldMesh() { Init(); }
		
		/// <summary>
		/// 頂点を生成する関数
		/// </summary>
		void CreateVertices() override { /* 3Dモデルでは削除する */ };
		/// <summary>
		/// インデックスを生成する関数
		/// </summary>
		void CreateIndexes() override { /* 3Dモデルでは削除する */ };
		
		/// <summary>
		/// モデルデータを読み込む
		/// </summary>
		/// <param name="filename">読み込むファイルの名前</param>
		void LoadFile(const std::string& filename);
		
		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const override { return static_cast<int>(vertices.size()); };
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		int GetIndexCount() const override { return static_cast<int>(indexes.size()); };


	public: // ** オペレーターオーバーロード ** //
		
		// コピー演算
		bool operator=(const OldMeshData& other) {
			// 描画する頂点
			vertices = other.vertices;
			// インデックス
			indexes = other.indexes;
			// マテリアル
			material = other.material;

			return this;
		};
	};
}