#pragma once

#include "Mesh.h"
#include "Node.h"

#include <vector>

namespace LWP::Primitive {
	/// <summary>
	/// 3Dモデル
	/// </summary>
	class ModelData final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// 3Dモデルのデータを読み込む
		/// </summary>
		/// <param name="filename">読み込むファイルの名前</param>
		void Load(const std::string& filename);
		

	private: // ** メンバ変数 ** //

		// メッシュ
		std::vector<Mesh> meshes_;
		// ノード
		std::vector<Node> nodes_;
	};
}