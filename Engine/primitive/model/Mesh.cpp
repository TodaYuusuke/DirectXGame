#include "Mesh.h"

#include "component/Resource.h"

using namespace LWP::Primitive;
using namespace LWP::Math;


void Mesh::Load(aiMesh* mesh) {
	// 名前を取得
	name = mesh->mName.C_Str();

	// texcoordを持っているか取得
	bool hasTexcoord = mesh->HasTextureCoords(0);
	// 法線を持っているか取得
	bool hasNormal = mesh->HasNormals();

	// マテリアルインデックス取得
	materialIndex = mesh->mMaterialIndex;

	// Vertexの解析
	for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
		Vertex newVertex;

		// インデックス情報を元に情報を取得する
		aiVector3D& position = mesh->mVertices[vertexIndex];		 // 頂点座標取得
		newVertex.position = { -position.x, position.y, position.z }; // 頂点座標追加

		// uv座標チェック
		if (hasTexcoord) {
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex]; // テクスチャ座標取得
			newVertex.texCoord = { texcoord.x, texcoord.y };			  // テクスチャ座標追加
		}
		else {
			newVertex.texCoord = { 0.0f,0.0f };
		}

		// 法線チェック
		if (hasNormal) {
			aiVector3D& normal = mesh->mNormals[vertexIndex];			 // 法線取得
			newVertex.normal = { -normal.x, normal.y, normal.z };		  // 法線追加
		}
		else {
			newVertex.normal = newVertex.position;
		}

		// マテリアル番号を頂点に保持
		newVertex.materialIndex = materialIndex;

		// 頂点データを追加
		vertices.push_back(newVertex);
	}

	// Face(Meshの中身)の解析
	for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
		aiFace& face = mesh->mFaces[faceIndex];
		assert(face.mNumIndices == 3);	// 三角形のみサポート

		// インデックス解析
		for (uint32_t element = 0; element < face.mNumIndices; element++) {
			uint32_t vertexIndex = face.mIndices[element];
			// インデックス情報を追加
			indexes.push_back(vertexIndex);
		}
	}

	// ボーン情報を解析する
	for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		// ボーンを取得
		aiBone* bone = mesh->mBones[boneIndex];
		// ジョイント名取得
		std::string jointName = bone->mName.C_Str();
		// そのジョイント名のスキンクラスター用データを取得
		JointWeight& jointWeightData = skinClusterData_[jointName];

		// バインドポーズ保存用の4x4行列をAssimpから取得
		aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
		// 保存用変数を定義
		aiVector3D scale, translate;
		aiQuaternion rotate;

		// バインドポーズ行列からSRTを抽出
		bindPoseMatrixAssimp.Decompose(scale, rotate, translate);

		// 抽出したSRTからアフィン変換行列の生成
		Matrix4x4 bindPoseMatrix = Matrix4x4::CreateAffineMatrix(
			Vector3(scale.x, scale.y, scale.z),
			Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w),
			Vector3(-translate.x, translate.y, translate.z)
		);
		// 求めたアフィン変換行列の逆行列を求める
		jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

		// ウェイト情報を解析する
		for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
			// ウェイト情報を追加
			jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			//vertices[bone->mWeights[weightIndex].mVertexId].weight = bone->mWeights[weightIndex].mWeight;
			//jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
		}
	}
}

void Mesh::Load(aiMesh* mesh, Skeleton& skeleton, SkinCluster& cluster) {
	Load(mesh);

	// Influenceを埋める
	for (const auto& jointWeight : skinClusterData_) {
		auto it = skeleton.jointMap.find(jointWeight.first);
		if (it == skeleton.jointMap.end()) {
			continue;
		}
		// (*it).secondにはJointのIndexが入っているので、該当のIndexのInverseBindPoseMatrixを代入
		cluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = vertices[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < Primitive::Vertex::kNumMaxInfluence; ++index) {
				if (currentInfluence.weight[index] == 0.0f) {
					currentInfluence.weight[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
}