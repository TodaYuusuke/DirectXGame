#include "Model.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;

InstanceData::InstanceData(const Resource::Model& value) {
	*this = value;
}
InstanceData& InstanceData::operator=(const Resource::Model& value) {
	wtf = value.worldTF;
	enableLighting = value.enableLighting;
	return *this;
}

Model::Model() {
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
	System::engine->resourceManager_->SetPointer(this);

	buffer.Init(System::engine->directXCommon_->GetGPUDevice());
}
Model::~Model() {
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
	System::engine->resourceManager_->DeletePointer(this);
}

void Model::Load(const std::string& fileName) {
	// resourcesフォルダ内から検索して読み込む処理を作る
	assert(false);	// 未実装のためassert
	fileName;
	// フルパスを作って読み込む
	//LoadFullPath();
}
void Model::LoadShortPath(const std::string& fp) {
	// フルパスにして読み込む
	LoadFullPath(kDirectoryPath + fp);
}
void Model::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
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
			for (uint32_t index = 0; index < Primitive::kNumMaxInfluence; ++index) {
				if (currentInfluence.weight[index] == 0.0f) {
					currentInfluence.weight[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
}

void Model::Update() {
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
		}
	}

	*buffer.data_ = *this;
}
void Model::Draw(Base::RendererManager* render) {
	if (!isActive) { return; }
	render;
	//render->AddModelData(GetModel(filePath), *this);
}
void Model::DebugGUI() {
	worldTF.DebugGUI();
	ImGui::Checkbox("enableLighting", &enableLighting);
	//ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}

// 短縮用パス
const std::string Model::kDirectoryPath = "resources/model/";
