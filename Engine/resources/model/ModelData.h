#pragma once

#include "primitive/model/Mesh.h"
#include "primitive/model/Node.h"
#include "primitive/model/Material.h"

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルの実体
	/// </summary>
	class ModelData final {
	public: // ** パブリックなメンバ変数 ** //

		// メッシュ
		std::vector<Primitive::Mesh> meshes_;
		// ノード
		std::vector<Primitive::Node> nodes_;
		// マテリアル
		std::vector<Primitive::Material> material_;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// 3Dモデルのデータを読み込む
		/// </summary>
		/// <param name="filename">読み込むファイルのパス</param>
		void Load(const std::string& filePath);

		/// <summary>
		/// 頂点数の合計を返す関数
		/// </summary>
		int GetVertexCount() {
			int size = 0;
			for (int i = 0; i < meshes_.size(); i++) {
				size += meshes_[i].GetVertexCount();
			}
			return size;
		}
		/// <summary>
		/// メッシュ数を返す関数
		/// </summary>
		int GetMeshCount() { return static_cast<int>(meshes_.size()); }
		/// <summary>
		/// マテリアル数を返す関数
		/// </summary>
		int GetMaterialCount() { return static_cast<int>(material_.size()); }
	};
}