#include "ModelData.h"

#include <cassert>

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Primitive;
using namespace LWP::Resource;


void ModelData::Load(const std::string& filePath) {
	Assimp::Importer importer;
	// ファイル読み込み（三角形の並び順を逆にする | UVをフリップする | 四角形以上のポリゴンを三角形に自動分割）
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
	assert(scene->HasMeshes());	// メッシュがないのは対応しない

	// メッシュレット生成用のデータ
	//std::vector<Base::VertexStruct> vertices;
	//std::vector<uint32_t> indexes;
	std::vector<Base::MaterialStruct> materials;

	// ノード情報を格納
	if (scene->mRootNode) {
		// 新しいノードを生成
		nodes_.emplace_back();
		// 読み込む
		nodes_.back().Load(scene->mRootNode);
		// スケルトンを作成
		skeleton_.emplace();
		skeleton_->Create(nodes_[0]);
		// skeletonからSkinClusterを生成
		skinCluster_.emplace(static_cast<uint32_t>(skeleton_->joints.size()));
	}


	// Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// 新しいメッシュを生成
		meshes_.emplace_back();
		// 読み込む
		Mesh& m = meshes_.back();
		// もしskeletonが作られているなら
		if (skeleton_.has_value()) {
			m.Load(mesh, skeleton_.value(), skinCluster_.value());
		}
		else {
			m.Load(mesh);
		}

		// 今読み込み済みの頂点数を保持
		int vertexCount = static_cast<int>(vertices.size());
		// 頂点をひとまとめに
		for (int i = 0; i < m.vertices.size(); i++) {
			vertices.push_back(m.vertices[i]);
		}
		// インデックスもひとまとめに
		for (int i = 0; i < m.indexes.size(); i++) {
			indexes.push_back(m.indexes[i] + vertexCount);
		}
	}

	// マテリアルの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		// マテリアルを作成
		std::string name = scene->mMaterials[materialIndex]->GetName().C_Str();
		// 無名であれば自動で名前を付ける
		if (name == "") {
			name = "Material" + std::to_string(materialIndex);
		}
		materials_[name].Load(scene->mMaterials[materialIndex], scene, filePath);
		materialOrder_.push_back(name);
	}
	// データ構造体に加工
	for (std::string str : materialOrder_) {
		materials.push_back(materials_[str]);
	}

	// 頂点座標格納用配列の作成
	auto vertPos = std::make_unique<DirectX::XMFLOAT3[]>(vertices.size());
	// 頂点座標を取得
	for (int i = 0; i < vertices.size(); i++) {
		vertPos[i] = { vertices[i].position_.x, vertices[i].position_.y, vertices[i].position_.z };
	}
	// メッシュレット作成時にでてくるデータの一時受け取り変数
	std::vector<DirectX::Meshlet> meshlets;
	std::vector<uint8_t> uniqueVertices;
	std::vector<DirectX::MeshletTriangle> primitiveIndices;

	// メッシュレットの変換成否確認用
	HRESULT result = S_FALSE;
	// 読み込んだモデルデータをメッシュレットに変換する
	result = DirectX::ComputeMeshlets(
		indexes.data(), indexes.size() / 3,
		vertPos.get(), vertices.size(),
		nullptr,
		meshlets,
		uniqueVertices,
		primitiveIndices
	);
	// 変換成否確認
	assert(SUCCEEDED(result));

	// メッシュレットの数を保持
	meshletCount_ = static_cast<int>(meshlets.size());

	// バッファの生成
	buffers_.meshlet = std::make_unique<StructuredBuffer<DirectX::Meshlet>>(static_cast<int32_t>(meshlets.size()));		// メッシュレット
	buffers_.vertex = std::make_unique<StructuredBuffer<Base::VertexStruct>>(static_cast<int32_t>(vertices.size()));	// 頂点
	buffers_.uniqueVertexIndices = std::make_unique<StructuredBuffer<uint32_t>>(static_cast<int32_t>(uniqueVertices.size()));	// 固有頂点
	buffers_.primitiveIndices = std::make_unique<StructuredBuffer<uint32_t>>(static_cast<int32_t>(primitiveIndices.size()));		// 形状プリミティブ
	buffers_.materials = std::make_unique<StructuredBuffer<Base::MaterialStruct>>(static_cast<int32_t>(materials.size()));		// マテリアル
	// バッファの初期化
	buffers_.meshlet->Init();
	buffers_.vertex->Init();
	buffers_.uniqueVertexIndices->Init();
	buffers_.primitiveIndices->Init();
	buffers_.materials->Init();
	// データのコピー
	std::memcpy(buffers_.meshlet->data_, meshlets.data(), sizeof(DirectX::Meshlet) * meshlets.size());
	for (VertexStruct& v : vertices) {
		buffers_.vertex->Add(v);
	}
	std::memcpy(buffers_.uniqueVertexIndices->data_, uniqueVertices.data(), sizeof(uint8_t) * uniqueVertices.size());
	for (DirectX::MeshletTriangle& v : primitiveIndices) {
		buffers_.primitiveIndices->Add(v.i0 | (v.i1 << 10) | (v.i2 << 20));
	}
	std::memcpy(buffers_.materials->data_, materials.data(), sizeof(Base::MaterialStruct) * materials.size());

}

std::vector<Vertex> ModelData::GetVertices() const {
	std::vector<Vertex> result;
	result.resize(GetVertexCount());
	for (const Mesh& m : meshes_) {
		result.insert(result.end(), m.vertices.begin(), m.vertices.end());
	}
	return result;
}

int ModelData::GetVertexCount() const {
	int size = 0;
	for (const Mesh& m : meshes_) {
		size += m.GetVertexCount();
	}
	return size;
}
