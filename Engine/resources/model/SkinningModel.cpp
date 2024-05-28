#include "SkinningModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;



SkinningModel::SkinningModel() {}
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

	// なければおかしいのでassert
	assert(data->skeleton_.has_value());

	// スケルトンをコピー
	skeleton = data->skeleton_.value();
	skinCluster = data->skinCluster_.value();

	// Wellを初期化
	wellBuffer = std::make_unique<Base::StructuredBuffer<Primitive::WellForGPU>>(static_cast<int32_t>(skeleton.joints.size()));
	wellBuffer->Init(System::engine->directXCommon_->GetGPUDevice(), System::engine->directXCommon_->GetSRV());
}

void SkinningModel::Update() {
	if (!isActive) { return; }

	// スケルトンがあるなら更新
	skeleton.Update();

	// SkinClusterの更新
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); jointIndex++) {
		assert(jointIndex < skinCluster->inverseBindPoseMatrices.size());
		Math::Matrix4x4 skeSpaceMatrix = skinCluster->inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster->mappedPalette[jointIndex].skeletonSpaceMatrix =
			skeSpaceMatrix;
		skinCluster->mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			skeSpaceMatrix.Inverse().Transpose();
		// Bufferのデータ更新
		wellBuffer->data_[jointIndex] = skinCluster->mappedPalette[jointIndex];
	}

	// データのコピー
	//std::memcpy(wellBuffer->data_, skinCluster->mappedPalette.data(), sizeof(Primitive::WellForGPU) * skinCluster->mappedPalette.size());
}
void SkinningModel::DebugGUI() {
	worldTF.DebugGUI();
	ImGui::Checkbox("enableLighting", &enableLighting);
	ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}
