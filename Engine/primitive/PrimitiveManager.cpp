#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include <Adapter.h>

using namespace LWP::Primitive;

void Manager::Initialize() {
	for (IPrimitive* p : primitives_) {
		delete p;
	}
	primitives_.clear();
	primitiveCountMap_.clear();
}

void Manager::Update() {
	// Debugビルド時のみImGuiを表示
#if _DEBUG
	// 生成用の関数ポインタ
	static std::vector<std::function<IPrimitive*()>> functions = {
		&LWP::Primitive::CreateInstance<Triangle>,
		&LWP::Primitive::CreateInstance<Surface>,
		&LWP::Primitive::CreateInstance<Sphere>,
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"Triangle","Surface","Sphere"
	};

	ImGui::Begin("PrimitiveManager");
	
	// 変更されて渡される値は添え字
	ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
	if (ImGui::Button("Create")) { functions[selectedClass](); }

	// 形状一覧
	if (!primitives_.empty()) {
		std::vector<const char*> itemText;
		for (IPrimitive* p : primitives_) {
			itemText.push_back(p->name.c_str());
		}
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
		primitives_[currentItem]->DebugGUI();
	}
	ImGui::End();
#endif
}

void Manager::Draw(Base::CommandManager* manager) {
	// 描画
	for (IPrimitive* p : primitives_) {
		p->Draw(manager);
	}
}