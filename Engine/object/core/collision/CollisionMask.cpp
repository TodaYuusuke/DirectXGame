#include "CollisionMask.h"

#include "base/ImGuiManager.h"
#include <string>

using namespace LWP;
using namespace LWP::Object;

void CollisionMask::DebugGUI() {
	// 所属フラグかヒットフラグかを選ぶ
	static int mode = 0;
	// 文字列（ほんとはConfigから名前を取得したい）
	static std::string str[] = {
		"Player",
		"Enemy",
		"Bullet",
		"Ground",
		"Particle",
		"Layer5",
		"Layer6",
		"Layer7",
		"Layer8",
		"Layer9",
		"Layer10",
		"Layer11",
		"Layer12",
		"Layer13",
		"FieldObject",
		"Terrain",
	};

	if (ImGui::TreeNode("Mask")) {
		// どちらのフラグを設定するか
		ImGui::RadioButton("Belong", &mode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Hit", &mode, 1);
		// 選択された方を参照に
		uint32_t* flag = nullptr;
		if (mode == 0) { flag = &belongFrag; }
		else { flag = &hitFrag; }

		uint32_t result = 0;
		for (int i = 0; i < 16; i++) {
			bool b = *flag & (0b1 << i);
			ImGui::Checkbox(str[i].c_str(), &b);
			result += (b << i);
		}
		*flag = result;

		// 一括変更ボタン
		if (ImGui::Button("ALL false")) {
			*flag = lwpC::Collider::MaskLayer::None;
		}
		if (ImGui::Button("ALL true")) {
			*flag = lwpC::Collider::MaskLayer::ALL;
		}
		ImGui::TreePop();
	}
}
