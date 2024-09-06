#include "SkinningModel.h"

#include "resources/ResourceManager.h"
#include "component/Resource.h"

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
	// マテリアルをコピー
	materials = data->materials_;

	// なければおかしいのでassert
	assert(data->skeleton_.has_value());

	// スケルトンをコピー
	skeleton = data->skeleton_.value();
	// スキンクラスターの参照を保持
	skinCluster = &data->skinCluster_.value();

	// Wellを初期化
	//wellBuffer = std::make_unique<Base::StructuredBuffer<Primitive::WellForGPU>>(static_cast<int32_t>(skeleton.joints.size()));
	//wellBuffer->Init(System::engine->directXCommon_->GetGPUDevice(), System::engine->directXCommon_->GetSRV());
}

void SkinningModel::Update() {
	if (!isActive) { return; }

	// スケルトンがあるなら更新
	skeleton.Update();
}
void SkinningModel::DebugGUI() {
	worldTF.DebugGUI();
	if (ImGui::TreeNode("Materials")) {
		for (auto itr = materials.begin(); itr != materials.end(); itr++) {
			itr->second.DebugGUI(itr->first);
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("isActive", &isActive);
	if (ImGui::Button("Change WireFrame")) { ChangeFillMode(); }
	if (ImGui::Button("Change All Lighting Flag true")) { SetAllMaterialLighting(true); }
	if (ImGui::Button("Change All Lighting Flag false")) { SetAllMaterialLighting(false); }
}

void SkinningModel::ChangeFillMode() {
	System::engine->resourceManager_->ChangeFillMode(this, filePath);
}

void SkinningModel::SetAllMaterialLighting(bool flag) {
	for (auto itr = materials.begin(); itr != materials.end(); itr++) {
		itr->second.enableLighting = flag;
	}
}

void SkinningModel::SetBufferData(Primitive::WellForGPU* data, int offset) {
	// 全Jointの計算結果を渡す
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); jointIndex++) {
		assert(jointIndex < skinCluster->inverseBindPoseMatrices.size());
		Math::Matrix4x4 skeSpaceMatrix = skinCluster->inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		// Bufferのデータ更新
		data[jointIndex + offset].skeletonSpaceMatrix = skeSpaceMatrix;
		data[jointIndex + offset].skeletonSpaceInverseTransposeMatrix = skeSpaceMatrix.Inverse().Transpose();
	}
}