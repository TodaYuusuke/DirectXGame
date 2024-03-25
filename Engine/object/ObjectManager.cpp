#include "ObjectManager.h"

#include <vector>
#include <functional>

#include "utility/MyUtility.h"

using namespace LWP;
using namespace LWP::Object;

void Manager::Initialize() {
	//for (IObject* o : objects_.list) {
	//	delete o;
	//}
	//objects_.clear();
	objectCountMap_.clear();
}

void Manager::Update(Base::CommandManager* manager) {
	// Debugビルド時のみImGuiを表示
#if DEMO
	// 生成用の関数ポインタ
	static std::vector<std::function<void()>> functions = {
		[this]() { debugPris.push_back(new Camera()); },
		[this]() { debugPris.push_back(new DirectionLight()); },
		[this]() { debugPris.push_back(new PointLight()); },
		[this]() { debugPris.push_back(new Particle()); },
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"Camera","DirectionLight","PointLight","Particle"
	};

	ImGui::Begin("LWP", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTabBar("LWP")) {
		if (ImGui::BeginTabItem("Object")) {
			// 変更されて渡される値は添え字
			ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
			if (ImGui::Button("Create")) { functions[selectedClass](); }

			// 形状一覧
			if (!objects_.list.empty()) {
				std::vector<const char*> itemText;
				for (IObject* o : objects_.list) {
					itemText.push_back(o->name.c_str());
				}
				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
				(*Utility::GetIteratorAtIndex<IObject*>(objects_.list, currentItem))->DebugGUI();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
#endif

	for (IObject* o : objects_.list) {
		o->Update(manager);
	}
}

void Manager::SetPointer(IObject* ptr) {
	objects_.SetPointer(ptr);

	// カウントのマップから数を測定し、デフォルトの名前を登録
	if (!objectCountMap_.count(ptr->name)) {
		// 存在しない場合のみ0で初期化
		objectCountMap_[ptr->name] = 0;
	}
	ptr->name += std::to_string(objectCountMap_[ptr->name]++);
}
void Manager::DeletePointer(IObject* ptr) {
	objects_.DeletePointer(ptr);
}