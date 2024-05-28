#pragma once
#include "Vertex.h"
#include "JointWeight.h"
#include "Skeleton.h"
#include "SkinCluster.h"

#include <vector>
#include <map>

namespace LWP::Primitive {
	/// <summary>
	/// 3Dモデルのノードクラス
	/// </summary>
	class Mesh final {
	public: // ** パブリックなメンバ関数 ** //

		// 名前
		std::string name = "mesh";

		// 描画する頂点
		std::vector<Vertex> vertices;
		// インデックス
		std::vector<uint32_t> indexes;
		// Jointの重さデータ
		std::map<std::string, Primitive::JointWeight> skinClusterData_;
		
		//　マテリアルのインデックス
		int materialIndex;


	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// Assimpのmeshデータから情報を読み込む
		/// </summary>
		/// <param name="aiMesh"></param>
		void Load(aiMesh* mesh);
		/// <summary>
		/// Assimpのmeshデータから情報を読み込む
		/// </summary>
		/// <param name="aiMesh"></param>
		void Load(aiMesh* mesh, Skeleton& skeleton, SkinCluster& cluster);

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() { return static_cast<int>(vertices.size()); }
		/// <summary>
		/// インデックス数を返す関数
		/// </summary>
		int GetIndexCount() { return static_cast<int>(indexes.size()); }

	};
}