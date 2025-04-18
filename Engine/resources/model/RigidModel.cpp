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
RigidModel::RigidModel(const RigidModel& other) {
	this->LoadFullPath(other.filePath);
	worldTF = other.worldTF;
	isActive = other.isActive;
}
RigidModel::~RigidModel() {
	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
		Resource::Manager::GetInstance()->DeletePointer(this, filePath);
	}
}

void RigidModel::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	// マテリアルをコピー
	materials = GetModel(filePath)->materials_;

	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	Resource::Manager::GetInstance()->SetPointer(this, filePath);
}


void RigidModel::Update() {
	if (!isActive) { return; }
}
void RigidModel::DebugGUI() {
	worldTF.DebugGUI();
	if (ImGui::TreeNode("Materials")) {
		for (auto itr = materials.begin(); itr != materials.end(); itr++) {
			itr->second.DebugGUI(itr->first);
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("isActive", &isActive);
	if(ImGui::Button("Change FillMode")) { ChangeFillMode(); }
	if (ImGui::Button("Change All Lighting Flag true")) { SetAllMaterialLighting(true); }
	if (ImGui::Button("Change All Lighting Flag false")) { SetAllMaterialLighting(false); }
}

void RigidModel::ChangeFillMode() {
	Resource::Manager::GetInstance()->ChangeFillMode(this, filePath);
}

void RigidModel::SetAllMaterialLighting(bool flag) {
	for (auto itr = materials.begin(); itr != materials.end(); itr++) {
		itr->second.enableLighting = flag;
	}
}

ModelData* RigidModel::GetModelData() const {
	return &*GetModel(filePath);
}
