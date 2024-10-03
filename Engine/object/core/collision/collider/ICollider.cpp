#include "ICollider.h"

#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;

void ICollider::DebugGUI() {
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
#if DEMO
	ImGui::Checkbox("isShowWireFrame", &isShowWireFrame);	// デバッグ用にWireFrameを表示するか
#endif
}