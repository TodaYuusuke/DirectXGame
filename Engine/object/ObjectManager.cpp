#include "ObjectManager.h"
#include "base/ImGuiManager.h"

#include <vector>
#include <functional>

#include "utility/MyUtility.h"

using namespace LWP;

namespace LWP::Object {
	void Manager::Init() {
		//for (IObject* o : objects_.list) {
		//	delete o;
		//}
		//objects_.clear();
		objectCount_ = 0;
		directionLight_.name = "DirectionLight";
		lightMetadataBuffer_.Init();
	}

	void Manager::Update() {
		for (auto& ptr : cameras_.list) { ptr->Update(); }
		for (auto& ptr : particle_.list) { ptr->Update(); }
		for (auto& ptr : spriteParticle_.list) { ptr->Update(); }
		for (auto& ptr : gpuParticle_.list) { ptr->Update(); }
		for (auto& ptr : particle_.list) { ptr->Update(); }
		for (auto& ptr : gpuParticle_.list) { ptr->Update(); }
		directionLight_.Update();
		for (auto& ptr : pointLight_.list) { ptr->Update(); }
	}

	void Manager::SetPtr(Camera* ptr) {
		cameras_.SetPtr(ptr);
		ptr->name = "Camera" + objectCount_++;
	}
	void Manager::SetPtr(Particle* ptr) {
		particle_.SetPtr(ptr);
		ptr->name = "Particle" + objectCount_++;
	}
	void Manager::SetPtr(SpriteParticle* ptr) {
		spriteParticle_.SetPtr(ptr);
		ptr->name = "SpriteParticle" + objectCount_++;
	}
	void Manager::SetPtr(GPUParticle* ptr) {
		gpuParticle_.SetPtr(ptr);
		ptr->name = "GPUParticle" + objectCount_++;
	}
	void Manager::SetPtr(PointLight* ptr) {
		pointLight_.SetPtr(ptr);
		ptr->name = "PointLight" + objectCount_++;
	}

	void Manager::DebugGUI() {
		// ImGuiを表示

		// 生成用の関数ポインタ
		static std::vector<std::function<void()>> functions = {
			[this]() { debugObjs_.push_back(new Camera()); },
			[this]() { /* Particleのクラスは動的生成をオフ */ },
			[this]() { /* SpriteParticleのクラスは動的生成をオフ */ },
			[this]() { /* GPUParticleのクラスは動的生成をオフ */ },
			[this]() { /* Terrainのクラスは動的生成をオフ */ },
			[this]() { /* DirectionLightのクラスは動的生成をオフ */ },
			[this]() { debugObjs_.push_back(new PointLight()); },
		};
		// 選択肢の変数
		static std::vector<const char*> classText = {
			"Camera","Particle","SpriteParticle","GPUParticle","Terrain","DirectionLight","PointLight"
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
				case LWP::Object::Manager::Type::SpriteParticle:
					drawListBox(spriteParticle_.list, false);
					break;
				case LWP::Object::Manager::Type::GPUParticle:
					drawListBox(gpuParticle_.list, false);
					break;
				case LWP::Object::Manager::Type::DirectionLight:
					directionLight_.DebugGUI();
					break;
				case LWP::Object::Manager::Type::PointLight:
					drawListBox(pointLight_.list, true);
					break;
			}
			ImGui::EndTabItem();
		}
	}
}