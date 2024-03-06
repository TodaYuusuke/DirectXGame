#include "ColliderManager.h"

#if DEMO
#include <component/Object.h>
#endif

using namespace LWP;
using namespace LWP::Object::Collider;

void Manager::Initialize() {
	for (ICollider* c : colliders_) {
		delete c;
	}
	colliders_.clear();
}

void Manager::Update() {
	// Debugビルド時のみImGuiを表示
#if DEMO
	// 生成用の関数ポインタ
	static std::vector<std::function<ICollider*()>> functions = {
		&LWP::Object::Collider::CreateInstance<AABB>,
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"AABB"
	};

	ImGui::Begin("LWP", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTabBar("LWP")) {
		if (ImGui::BeginTabItem("Collider")) {
			// 変更されて渡される値は添え字
			ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
			if (ImGui::Button("Create")) { 
				functions[selectedClass]();
			}

			// 形状一覧
			if (!colliders_.empty()) {
				std::vector<const char*> itemText;
				for (ICollider* c : colliders_) {
					itemText.push_back(c->name.c_str());
				}
				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
				colliders_[currentItem]->DebugGUI();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	// デバッグしやすいようにワイヤーフレームを描画
	for (ICollider* c : colliders_) {
		c->ShowWireFrame(LWP::System::engine->directXCommon_->GetCommandManager());
	}
#endif

}