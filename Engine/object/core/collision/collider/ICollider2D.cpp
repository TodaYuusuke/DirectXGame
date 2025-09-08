#include "ICollider2D.h"

#include "base/ImGuiManager.h"
#if DEMO
#include "primitive/2d/planeInterface/Sprite.h"
#endif

using namespace LWP::Object::Collider2D;

void ICollider::DebugGUI() {
#if DEMO
	if (ImGui::TreeNode("Sprite")) {
		sprite_.DebugGUI();
		ImGui::TreePop();
	}
#endif
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
}

void ICollider::SetFollowPtr(Object::TransformQuat* ptr) {
	tf_ = ptr;
}
