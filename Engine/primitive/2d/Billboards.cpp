#include "Billboards.h"
#include "base/ImGuiManager.h"

namespace LWP::Primitive {
	void IStretchedBillboard::ChildDebugGUI() {
		ImGui::DragFloat3("Velocity", &velocity.x, 0.01f);
	}
}