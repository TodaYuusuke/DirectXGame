#include "ObjectManager.h"
#include "base/ImGuiManager.h"

#include <vector>
#include <functional>

#include "utility/MyUtility.h"

using namespace LWP;
using namespace LWP::Object;

void Manager::Init() {
	//for (IObject* o : objects_.list) {
	//	delete o;
	//}
	//objects_.clear();
	objectCount_ = 0;
}

void Manager::Update(Base::RendererManager* manager) {
	for (auto& ptr : cameras_.list) { ptr->Update(manager); }
	for (auto& ptr : particle_.list) { ptr->Update(manager); }
	for (auto& ptr : gpuParticle_.list) { ptr->Update(manager); }
	for (auto& ptr : particle_.list) { ptr->Update(manager); }
	for (auto& ptr : gpuParticle_.list) { ptr->Update(manager); }
	for (auto& ptr : terrain_.list) { ptr->Update(manager); }
	for (auto& ptr : directionLight_.list) { ptr->Update(manager); }
	for (auto& ptr : pointLight_.list) { ptr->Update(manager); }
}

void Manager::DebugGUI() {
	// ImGuiを表示

	// 生成用の関数ポインタ
	static std::vector<std::function<void()>> functions = {
		[this]() { debugObjs_.push_back(new Camera()); },
		[this]() { /* Particleのクラスは動的生成をオフ */ },
		[this]() { /* GPUParticleのクラスは動的生成をオフ */ },
		[this]() { /* Terrainのクラスは動的生成をオフ */ },
		[this]() { debugObjs_.push_back(new DirectionLight()); },
		[this]() { debugObjs_.push_back(new PointLight()); },
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"Camera","Particle","GPUParticle","Terrain","DirectionLight","PointLight"
	};
	static auto drawListBox = [this](auto& list, bool allowCreate) {
		using T = std::remove_pointer_t<typename std::decay_t<decltype(list)>::value_type>;
		if (allowCreate && ImGui::Button("Create new Instance")) { functions[selectedClass](); }	// 新しいインスタンスを作成するボタン
		if (!list.empty()) {
			std::vector<const char*> itemText;
			for (T* obj : list) {
				itemText.push_back(obj->name.c_str());
			}

			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

			auto it = Utility::GetIteratorAtIndex<T*>(list, currentItem);
			if (it != list.end()) {
				(*it)->DebugGUI(); // 共通インタフェースがある前提
			}
		}
	};

	if (ImGui::BeginTabItem("Object")) {
		// 変更されて渡される値は添え字
		ImGui::Combo("Select Type", &selectedClass, classText.data(), static_cast<int>(classText.size()));

		switch (Type(selectedClass)) {
			case LWP::Object::Manager::Type::Camera:
				drawListBox(cameras_.list, true);
				break;
			case LWP::Object::Manager::Type::Particle:
				drawListBox(particle_.list, false);
				break;
			case LWP::Object::Manager::Type::GPUParticle:
				drawListBox(gpuParticle_.list, false);
				break;
			case LWP::Object::Manager::Type::Terrain:
				drawListBox(terrain_.list, false);
				break;
			case LWP::Object::Manager::Type::DirectionLight:
				drawListBox(directionLight_.list, true);
				break;
			case LWP::Object::Manager::Type::PointLight:
				drawListBox(pointLight_.list, true);
				break;
		}
		ImGui::EndTabItem();
	}
}