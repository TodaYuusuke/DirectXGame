#pragma once
#include <DirectXMesh/DirectXMesh.h>
#include "base/directX/resource/data/StructuredBuffer.h"

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
		std::vector<Primitive::Material> materials_;
		// スケルトン
		std::optional<Primitive::Skeleton> skeleton_{};
		// スキンクラスター
		std::optional<Primitive::SkinCluster> skinCluster_{};


		// データバッファ群
		struct Buffers {
			//Base::ConstantBuffer<Math::Matrix4x4> transform;		// ワールドトランスフォーム
			std::unique_ptr<Base::StructuredBuffer<DirectX::Meshlet>> meshlet;	// メッシュレット
			std::unique_ptr<Base::StructuredBuffer<Base::VertexStruct>> vertex;	// 頂点
			std::unique_ptr<Base::StructuredBuffer<uint32_t>> uniqueVertexIndices;// 固有頂点インデックス
			std::unique_ptr<Base::StructuredBuffer<uint32_t>> primitiveIndices;	// プリミティブインデックス
			std::unique_ptr<Base::StructuredBuffer<Base::MaterialStruct>> materials;	// マテリアル
		}buffers_;

	private: // ** メンバ変数 ** //

		// メッシュレットの数を保持
		int meshletCount_ = 0;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ModelData() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~ModelData() = default;


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
		/// メッシュレット数を返す関数
		/// </summary>
		int GetMeshletCount() { return meshletCount_; }
	};
}