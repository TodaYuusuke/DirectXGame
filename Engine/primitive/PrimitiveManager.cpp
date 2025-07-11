#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "Config.h"
#include "utility/MyUtility.h"

namespace LWP::Primitive {
	void Manager::Init() {
		//for (IPrimitive* p : primitives_.list) {
		//	delete p;
		//}
		//primitives_.list.clear();
		primitiveCount_ = 0;

		for (IPrimitive* p : debugPris) {
			delete p;
		}
		debugPris.clear();
	}

	void Manager::Update() {
		// 全てのバッファーをリセット
		spriteBuffers_.Reset();

		// 更新処理
		for (ISprite* p : sprites_.list) {
			// primitiveを更新
			p->Update();

			// バッファーにデータをセット
			if (p->isActive) {
				for (const Vertex& v : p->vertices) { spriteBuffers_.vertices.Add(v); }
				spriteBuffers_.materials.Add(p->material);
				spriteBuffers_.wtf.Add(p->worldTF);
				spriteBuffers_.count++;
			}
		}
	}

	void Manager::DebugGUI() {
		// どの種類のPrimitiveのリストを表示するか
		static std::vector<const char*> typeText = {
			"Sprite",
		};
		static int typeID = 0;

		if (ImGui::BeginTabItem("Primitive")) {
			// 変更されて渡される値は添え字
			ImGui::Combo("Select Type", &typeID, typeText.data(), static_cast<int>(typeText.size()));

			switch (Type(typeID)) {
				case Type::Sprite:
					SpriteDebugGUI();
					break;
			}

			ImGui::EndTabItem();
		}
	}
	
	void Manager::SetPtr(ISprite* ptr) {
		ptr->name = std::to_string(primitiveCount_++);	// 数字だけ渡す
		sprites_.SetPtr(ptr);
	}

	Manager::PlaneBuffers::PlaneBuffers() 
		: vertices(lwpC::Rendering::Primitive::Sprite::kMaxCount * 4),
		  wtf(lwpC::Rendering::Primitive::Sprite::kMaxCount),
		  materials(lwpC::Rendering::Primitive::Sprite::kMaxCount) {
		vertices.Init();
		wtf.Init();
		materials.Init();
	}
	void Manager::PlaneBuffers::Reset() {
		vertices.Reset();
		wtf.Reset();
		materials.Reset();
		count = 0;
	}

	void Manager::SpriteDebugGUI() {
		static std::vector<std::function<void()>> functions = {
			[this]() { debugPris.push_back(new NormalSprite()); },
			[this]() { debugPris.push_back(new SequenceSprite()); },
		};
		// 選択肢の変数
		static std::vector<const char*> classText = {
			"NormalSprite","SequenceSprite"
		};
		static int classID = 0;
		static int currentItem = 0;

		// 変更されて渡される値は添え字
		ImGui::Combo("New Instance Type", &classID, classText.data(), static_cast<int>(classText.size()));
		if (ImGui::Button("Create")) { functions[classID](); }

		// インスタンス一覧
		if (!sprites_.list.empty()) {
			std::vector<const char*> itemText;
			for (ISprite* p : sprites_.list) {
				itemText.push_back(p->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			auto it = Utility::GetIteratorAtIndex(sprites_.list, currentItem);
			if (it != sprites_.list.end()) { (*it)->DebugGUI(); } // リスト内にあったなら呼び出す
		}
	}
}