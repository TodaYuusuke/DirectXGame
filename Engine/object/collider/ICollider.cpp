#include "ICollider.h"
#include "base/ImGuiManager.h"

#include "component/Object.h"

using namespace LWP::Object::Collider;

// コンストラクタ
ICollider::ICollider() {
	SetPointer(this);
}
// デストラクタ
ICollider::~ICollider() {
	if (follow_.t) {
		delete follow_.t;
	}

	DeletePointer(this);
}

void ICollider::Update() {
	// preHit更新
	preHit = hit;
	hit = false;	// この後にヒット判定をするのでとりまfalse

	// アクティブがOffのとき
	if (!isActive) { return; }
	// 派生先の更新
	UpdateShape();
}

void ICollider::DebugGUI() {
	// 派生クラス用
	DerivedDebugGUI();
	ImGui::Text("- Below this are common variables - ");
	// 追従先のワールドトランスフォーム
	if (follow_.t && ImGui::TreeNode("FollowTarget Info")) {
		follow_.t->DebugGUI();
		ImGui::TreePop();
	}
	
	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
#if DEMO
	ImGui::Checkbox("isShowWireFrame", &isShowWireFrame);	// デバッグ用にWireFrameを表示するか
#endif
}