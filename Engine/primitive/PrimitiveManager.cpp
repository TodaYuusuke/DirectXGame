#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "Config.h"
#include "utility/MyUtility.h"

namespace LWP::Primitive {
	PlaneBuffers::PlaneBuffers()
		: vertices(lwpC::Rendering::Primitive::Sprite::kMaxCount * 4),
		wtf(lwpC::Rendering::Primitive::Sprite::kMaxCount),
		materials(lwpC::Rendering::Primitive::Sprite::kMaxCount) {
		vertices.Init();
		wtf.Init();
		materials.Init();
		count = 0;
	}
	void PlaneBuffers::Reset() {
		vertices.Reset();
		wtf.Reset();
		materials.Reset();
		count = 0;
	}

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
		billboard3DBuffers_.Reset();

		// 更新処理（スプライト）
		for (IPlane* p : sprites_.list) {
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
		// 更新処理
		for (IPlane* p : billboards3D_.list) {
			// primitiveを更新
			p->Update();

			// バッファーにデータをセット
			if (p->isActive) {
				for (const Vertex& v : p->vertices) { billboard3DBuffers_.vertices.Add(v); }
				billboard3DBuffers_.materials.Add(p->material);
				billboard3DBuffers_.wtf.Add(p->worldTF);
				billboard3DBuffers_.count++;
			}
		}
	}

	void Manager::DebugGUI() {
		// どの種類のPrimitiveのリストを表示するか
		static std::vector<const char*> typeText = {
			"Sprite", "Billboard",
		};
		static int typeID = 0;

		if (ImGui::BeginTabItem("Primitive")) {
			// 変更されて渡される値は添え字
			ImGui::Combo("Select Type", &typeID, typeText.data(), static_cast<int>(typeText.size()));

			switch (Type(typeID)) {
				case Type::Sprite:
					SpriteDebugGUI();
					break;
				case Type::Billboard:
					BillboardDebugGUI();
					break;
			}

			ImGui::EndTabItem();
		}
	}

	void Manager::SetSpritePtr(IPlane* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		sprites_.SetPtr(ptr);
	}
	void Manager::SetBillboard3DPtr(IPlane* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		billboards3D_.SetPtr(ptr);
	}

	void Manager::SpriteDebugGUI() {
		static std::vector<std::function<void()>> functions = {
			[this]() { debugPris.push_back(new NormalSprite()); },
			[this]() { debugPris.push_back(new SequenceSprite()); },
			[this]() { debugPris.push_back(new ClipSprite()); },
		};
		// 選択肢の変数
		static std::vector<const char*> classText = {
			"NormalSprite","SequenceSprite","ClipSprite"
		};
		static int classID = 0;
		static int currentItem = 0;

		// 変更されて渡される値は添え字
		ImGui::Combo("New Instance Type", &classID, classText.data(), static_cast<int>(classText.size()));
		if (ImGui::Button("Create")) { functions[classID](); }

		// インスタンス一覧
		if (!sprites_.list.empty()) {
			std::vector<const char*> itemText;
			for (IPlane* p : sprites_.list) {
				itemText.push_back(p->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			auto it = Utility::GetIteratorAtIndex(sprites_.list, currentItem);
			if (it != sprites_.list.end()) { (*it)->DebugGUI(); } // リスト内にあったなら呼び出す
		}
	}
	void Manager::BillboardDebugGUI() {
		static std::vector<std::function<void()>> functions = {
			[this]() { debugPris.push_back(new NormalBillboard3D()); },
			[this]() { debugPris.push_back(new SequenceBillboard3D()); },
			[this]() { debugPris.push_back(new ClipBillboard3D()); },
		};
		// 選択肢の変数
		static std::vector<const char*> classText = {
			"NormalBillboard3D","SequenceBillboard3D","ClipBillboard3D"
		};
		static int classID = 0;
		static int currentItem = 0;

		// 変更されて渡される値は添え字
		ImGui::Combo("New Instance Type", &classID, classText.data(), static_cast<int>(classText.size()));
		if (ImGui::Button("Create")) { functions[classID](); }

		// インスタンス一覧
		if (!billboards3D_.list.empty()) {
			std::vector<const char*> itemText;
			for (IPlane* p : billboards3D_.list) {
				itemText.push_back(p->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			auto it = Utility::GetIteratorAtIndex(billboards3D_.list, currentItem);
			if (it != billboards3D_.list.end()) { (*it)->DebugGUI(); } // リスト内にあったなら呼び出す
		}
	}
}