#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace LWP::Primitive {
	/// <summary>
	/// 3Dモデルを読み込む
	/// </summary>
	class Model final {
	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// モデルデータを読み込む
		/// </summary>
		/// <param name="filename">読み込むファイルの名前</param>
		void LoadFile(const std::string& filename);
		


	public: // ** オペレーターオーバーロード ** //
		
		// コピー演算
		bool operator=(const MeshData& other) {
			// 描画する頂点
			vertices = other.vertices;
			// インデックス
			indexes = other.indexes;
			// マテリアル
			material = other.material;
			// テクスチャ
			texture = other.texture;
			// ノード
			node = other.node;

			return this;
		};
	};
}