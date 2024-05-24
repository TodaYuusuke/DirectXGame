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
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	System::engine->resourceManager_->SetPointer(this, filePath);
}

void RigidModel::Update() {
	if (!isActive) { return; }
	*buffer.data_ = *this;	// データ更新
}
void RigidModel::DebugGUI() {
	worldTF.DebugGUI();
	ImGui::Checkbox("enableLighting", &enableLighting);
	ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}