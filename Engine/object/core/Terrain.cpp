#include "Terrain.h"

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Resource;

Terrain::Terrain() : collider_(collision.SetBroadShape(Collider::Terrain())) {
	Initialize();
}

// 初期化
void Terrain::Initialize() {
	collision.mask.SetBelongFrag(lwpC::Collider::Terrain);
	collision.mask.SetHitFrag(lwpC::Collider::None);
	collision.name = "Terrain";
}
// 更新
void Terrain::Update(Base::RendererManager* manager) {
	manager;

	// 自身のisActiveをモデルとコライダーに適応
	model_.isActive = isActive;
	collision.isActive = isActive;
}

void Terrain::LoadModel(std::string filePath, const TransformQuat& wtf) {
	// モデル読み込み
	model_.LoadShortPath(filePath);
	model_.ApplyWorldTransform(wtf);

	// コライダーを作成
	collider_.Create(model_);
}

void Terrain::DebugGUI() {
	if (ImGui::TreeNode("Model")) {
		model_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Collision")) {
		collision.DebugGUI();
		ImGui::TreePop();
	}
	ImGui::Checkbox("isActive", &isActive);
}
