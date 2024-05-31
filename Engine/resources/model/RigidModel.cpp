#include "RigidModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;


RigidModel::RigidModel() {}
RigidModel::RigidModel(const RigidModel& other) : RigidModel() {
	this->LoadFullPath(other.filePath);
	worldTF = other.worldTF;
	enableLighting = other.enableLighting;
	isActive = other.isActive;
}
RigidModel::~RigidModel() {
	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
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

	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	System::engine->resourceManager_->SetPointer(this, filePath);
}

void RigidModel::Update() {
	if (!isActive) { return; }
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
	ImGui::Checkbox("isActive", &isActive);
	if(ImGui::Button("Change WireFrame")) { ChangeFillMode(); }
}

void RigidModel::ChangeFillMode() {
	System::engine->resourceManager_->ChangeFillMode(this, filePath);
}