#include "SkinningModel.h"

#include "resources/ResourceManager.h"
#include "component/Resource.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;
using namespace LWP::Utility;


SkinningModel::SkinningModel() {
}
SkinningModel::~SkinningModel() {
	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		Resource::Manager::GetInstance()->DeletePointer(this, filePath);
	}
}

void SkinningModel::LoadFullPath(const std::string& fp) {
	assert(filePath.empty() && "The model is already loaded.");	// 既に読み込まれている場合はエラー

	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	// ポインタをエンジンにセット
	Resource::Manager::GetInstance()->SetPointer(this, filePath);
	ModelData* data = GetModel(filePath);
	// マテリアルをコピー
	materials = data->materials_;

	// なければおかしいのでassert
	assert(data->skeleton_.has_value());

	// スケルトンをコピー
	skeleton = data->skeleton_.value();
	skeleton.SetParent();
	skeleton.Update();		// Updateを呼び出して座標の情報を初期化
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
	if (ImGui::TreeNode("Node")) {
		for (Primitive::Joint& joint : skeleton.joints) {
			if (ImGui::TreeNode(joint.name.c_str())) {
				joint.localTF.DebugGUI();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("isActive", &isActive);
	if (ImGui::Button("Change WireFrame")) { ChangeFillMode(); }
	if (ImGui::Button("Change All Lighting Flag true")) { SetAllMaterialLighting(true); }
	if (ImGui::Button("Change All Lighting Flag false")) { SetAllMaterialLighting(false); }
}

Primitive::Joint* SkinningModel::GetJoint(const std::string& name) {
	assert(skeleton.jointMap.contains(name));	// Jointの存在チェック
	return &skeleton.joints[skeleton.jointMap[name]];
}
Matrix4x4 SkinningModel::GetJointWorldMatrix(const std::string& name) {
	return GetJoint(name)->skeletonSpaceMatrix * worldTF.GetAffineMatrix();
}
Vector3 SkinningModel::GetJointWorldPosition(const std::string& name) {
	Math::Matrix4x4 worldMatrix = GetJointWorldMatrix(name);
	return { worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };
}
void SkinningModel::ChangeFillMode() {
	Resource::Manager::GetInstance()->ChangeFillMode(this, filePath);
}

void SkinningModel::SetAllMaterialLighting(bool flag) {
	for (auto itr = materials.begin(); itr != materials.end(); itr++) {
		itr->second.enableLighting = flag;
	}
}

void SkinningModel::SetBufferData(Base::StructuredBuffer<Primitive::WellForGPU>* data) {
	// 全Jointの計算結果を渡す
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); jointIndex++) {
		assert(jointIndex < skinCluster->inverseBindPoseMatrices.size());
		Math::Matrix4x4 skeSpaceMatrix = skinCluster->inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		// Bufferのデータ更新
		data->Add(Primitive::WellForGPU{
			skeSpaceMatrix,
			skeSpaceMatrix.Inverse().Transpose()
		});
		//data[jointIndex + offset].skeletonSpaceMatrix = skeSpaceMatrix;
		//data[jointIndex + offset].skeletonSpaceInverseTransposeMatrix = skeSpaceMatrix.Inverse().Transpose();
	}
}