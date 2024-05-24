#include "SkinningModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;

InstanceSkinData::InstanceSkinData(const Resource::SkinningModel& value) {
	*this = value;
}
InstanceSkinData& InstanceSkinData::operator=(const Resource::SkinningModel& value) {
	wtf = value.worldTF;
	enableLighting = value.enableLighting;
	return *this;
}

SkinningModel::SkinningModel() {
	// バッファ作成
	buffer.Init(System::engine->directXCommon_->GetGPUDevice());
}
SkinningModel::~SkinningModel() {
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略

	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		System::engine->resourceManager_->DeletePointer(this, filePath);
	}
}

void SkinningModel::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	System::engine->resourceManager_->SetPointer(this, filePath);
	ModelData* data = GetModel(filePath);

	// スケルトン生成
	skeleton.emplace();
	skeleton->Create(data->nodes_[0]);

	// skeletonからSkinClusterを生成
	skinCluster.emplace(static_cast<uint32_t>(skeleton->joints.size()));
	// Influenceを埋める
	for (const auto& jointWeight : data->meshes_[0].skinClusterData_) {
		auto it = skeleton->jointMap.find(jointWeight.first);
		if (it == skeleton->jointMap.end()) {
			continue;
		}
		// (*it).secondにはJointのIndexが入っているので、該当のIndexのInverseBindPoseMatrixを代入
		skinCluster->inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = data->meshes_[0].vertices[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < Primitive::Vertex::kNumMaxInfluence; ++index) {
				if (currentInfluence.weight[index] == 0.0f) {
					currentInfluence.weight[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	// Wellを初期化
	wellBuffer = std::make_unique< Base::StructuredBuffer<Primitive::WellForGPU>>(static_cast<int32_t>(skeleton->joints.size()));
	wellBuffer->Init(System::engine->directXCommon_->GetGPUDevice(), System::engine->directXCommon_->GetSRV());
}

void SkinningModel::Update() {
	if (!isActive) { return; }

	// スケルトンがあるなら更新
	if (skeleton.has_value()) {
		skeleton->Update();

		// SkinClusterの更新
		for (size_t jointIndex = 0; jointIndex < skeleton->joints.size(); jointIndex++) {
			assert(jointIndex < skinCluster->inverseBindPoseMatrices.size());
			Math::Matrix4x4 skeSpaceMatrix = skinCluster->inverseBindPoseMatrices[jointIndex] * skeleton->joints[jointIndex].skeletonSpaceMatrix;
			skinCluster->mappedPalette[jointIndex].skeletonSpaceMatrix = 
				skeSpaceMatrix;
			skinCluster->mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = 
				skeSpaceMatrix.Inverse().Transpose();
			// Bufferのデータ更新
			*wellBuffer->data_ = skinCluster->mappedPalette[jointIndex];
		}
	}

	*buffer.data_ = *this;
	// データのコピー
	//std::memcpy(wellBuffer->data_, skinCluster->mappedPalette.data(), sizeof(Primitive::WellForGPU) * skinCluster->mappedPalette.size());
}
void SkinningModel::DebugGUI() {
	worldTF.DebugGUI();
	ImGui::Checkbox("enableLighting", &enableLighting);
	ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}
