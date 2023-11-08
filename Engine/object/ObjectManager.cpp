#include "ObjectManager.h"

#include <Adapter.h>
#include <vector>
#include <functional>

using namespace LWP;
using namespace LWP::Object;

void Manager::Initialize() {
	for (IObject* o : objects_) {
		delete o;
	}
	objects_.clear();
	objectCountMap_.clear();
}

void Manager::Update(Base::CommandManager* manager) {
	// Debugビルド時のみImGuiを表示
#if _DEBUG
	// 生成用の関数ポインタ
	static std::vector<std::function<IObject*()>> functions = {
		&LWP::Object::CreateInstance<Camera>,
		&LWP::Object::CreateInstance<PointLight>,
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"Camera","PointLight"
	};

	ImGui::Begin("ObjectManager");

	// 変更されて渡される値は添え字
	ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
	if (ImGui::Button("Create")) { functions[selectedClass](); }

	// 形状一覧
	std::vector<const char*> itemText;
	for (IObject* o : objects_) {
		itemText.push_back(o->name.c_str());
	}
	ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
	objects_[currentItem]->DebugGUI();

	ImGui::End();
#endif

	for (IObject* o : objects_) {
		o->Update(manager);
	}
}