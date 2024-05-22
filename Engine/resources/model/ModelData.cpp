#include "ModelData.h"

#include <cassert>

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Primitive;
using namespace LWP::Resource;


void ModelData::Load(const std::string& filePath) {
	// バッファ生成のためにデバイスとSRVを取得する
	GPUDevice* device = System::engine->directXCommon_->GetGPUDevice();
	SRV* srv = System::engine->directXCommon_->GetSRV();

	Assimp::Importer importer;
	// ファイル読み込み（三角形の並び順を逆にする | UVをフリップする | 四角形以上のポリゴンを三角形に自動分割）
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
	assert(scene->HasMeshes());	// メッシュがないのは対応しない

	// メッシュレット生成用のデータ
	std::vector<Base::VertexStruct> vertices;
	std::vector<uint32_t> indexes;
	std::vector<Base::MaterialStruct> materials;

	// Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// 新しいメッシュを生成
		meshes_.emplace_back();
		// 読み込む
		Mesh& m = meshes_.back();
		m.Load(mesh);

		// 頂点をひとまとめに
		for (int i = 0; i < m.vertices.size(); i++) {
			vertices.push_back(m.vertices[i].v);
		}
		// インデックスもひとまとめに
		for (int i = 0; i < m.indexes.size(); i++) {
			indexes.push_back(m.indexes[i]);
		}
	}

	// ノード情報を格納
	if (scene->mRootNode) {
		// 新しいノードを生成
		nodes_.emplace_back();
		// 読み込む
		nodes_.back().Load(scene->mRootNode);
	}

	// マテリアルの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		// 新しいメッシュを生成
		materials_.emplace_back();
		// 読み込む
		materials_.back().Load(scene->mMaterials[materialIndex], scene, filePath);
	}
	// データ構造体に加工
	for (int i = 0; i < materials_.size(); i++) {
		materials.push_back(materials_[i]);
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
	buffers_.materials = std::make_unique<StructuredBuffer<Base::MaterialStruct>>(static_cast<int32_t>(materials.size()));		// 形状プリミティブ
	// バッファの初期化
	buffers_.meshlet->Init(device, srv);
	buffers_.vertex->Init(device, srv);
	buffers_.uniqueVertexIndices->Init(device, srv);
	buffers_.primitiveIndices->Init(device, srv);
	buffers_.materials->Init(device, srv);
	// データのコピー
	std::memcpy(buffers_.meshlet->data_, meshlets.data(), sizeof(DirectX::Meshlet) * meshlets.size());
	std::memcpy(buffers_.vertex->data_, vertices.data(), sizeof(Base::VertexStruct) * vertices.size());
	std::memcpy(buffers_.uniqueVertexIndices->data_, uniqueVertices.data(), sizeof(uint8_t) * uniqueVertices.size());
	std::memcpy(buffers_.primitiveIndices->data_, primitiveIndices.data(), sizeof(DirectX::MeshletTriangle) * primitiveIndices.size());
	std::memcpy(buffers_.materials->data_, materials.data(), sizeof(Base::MaterialStruct) * materials.size());
}