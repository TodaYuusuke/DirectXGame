#include "RigidModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;

InstanceRigidData::InstanceRigidData(const Resource::RigidModel& value) {
	*this = value;
}
InstanceRigidData& InstanceRigidData::operator=(const Resource::RigidModel& value) {
	wtf = value.worldTF;
	enableLighting = value.enableLighting;
	return *this;
}

RigidModel::RigidModel() {
	// バッファ作成
	buffer.Init(System::engine->directXCommon_->GetGPUDevice());
}
RigidModel::~RigidModel() {
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略

	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		System::engine->resourceManager_->DeletePointer(this, filePath);
	}
}

void RigidModel::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	// マテリアルをコピー
	ModelData* data = GetModel(filePath);
	materials.resize(data->materials_.size());
	std::copy(data->materials_.begin(), data->materials_.end(), materials.begin());

	// MaterialBufferを初期化
	mBuffers = std::make_unique<Base::StructuredBuffer<Base::MaterialStruct>>(static_cast<int32_t>(materials.size()));
	mBuffers->Init(System::engine->directXCommon_->GetGPUDevice(), System::engine->directXCommon_->GetSRV());

	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	System::engine->resourceManager_->SetPointer(this, filePath);
}

void RigidModel::Update() {
	if (!isActive) { return; }
	// データ更新
	*buffer.data_ = *this;
	// データ構造体に加工
	for (int i = 0; i < materials.size(); i++) {
		mBuffers->data_[i] = materials[i];
	}
}
void RigidModel::DebugGUI() {
	worldTF.DebugGUI();
	if (ImGui::TreeNode("Materials")) {
		for (int i = 0; i < materials.size(); i++) {
			materials[i].DebugGUI(std::to_string(i));
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("enableLighting", &enableLighting);
	ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}