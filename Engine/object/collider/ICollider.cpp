#include "ICollider.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;

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
	if (follow_.t) { follow_.t->DebugGUI(); }
	
	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
#if DEMO
	ImGui::Checkbox("isShowWireFrame", &isShowWireFrame);	// デバッグ用にWireFrameを表示するか
#endif
}