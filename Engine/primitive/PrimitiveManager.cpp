#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "object/ObjectManager.h"
#include "Config.h"
#include "utility/MyUtility.h"

using namespace LWP::Math;

namespace LWP::Primitive {

	Manager::Manager() :
		zSort_(lwpC::Rendering::Primitive::Billboaed::kMaxCount) {
		zSort_.Init();
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
		billboardBuffers_.Reset();
		zSort_.Reset();

		int i = 0;
		std::vector<BillboardIndex> indexes;
		Math::Vector3 cameraPos = Object::Manager::GetInstance()->GetMainCamera()->worldTF.GetWorldPosition();

		// 更新処理（スプライト）
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
		// 更新処理（ビルボード）
		for (IBillboard2D* p : billboards_.list) {
			// primitiveを更新
			p->Update();

			// バッファーにデータをセット
			if (p->isActive) {
				for (const Vertex& v : p->vertices) { billboardBuffers_.vertices.Add(v); }
				billboardBuffers_.materials.Add(p->material);
				billboardBuffers_.wtf.Add(p->worldTF);
				billboardBuffers_.count++;

				float d = Vector3::Distance(p->GetCenterPosition(), cameraPos);
				indexes.push_back({ i++, d });
			}
		}

		// Zソート
		std::sort(indexes.begin(), indexes.end(), 
			[](const BillboardIndex& a, const BillboardIndex& b) {
				return a.distance > b.distance;
			}
		);
		// ソート結果をバッファに格納
		for (BillboardIndex& b : indexes) { zSort_.Add(b.index); }
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

	void Manager::SetSpritePtr(ISprite* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		sprites_.SetPtr(ptr);
	}
	void Manager::SetBillboardPtr(IBillboard2D* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		billboards_.SetPtr(ptr);
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
			[this]() { debugPris.push_back(new NormalBillboard2D()); },
			[this]() { debugPris.push_back(new SequenceBillboard2D()); },
			[this]() { debugPris.push_back(new ClipBillboard2D()); },
		};
		// 選択肢の変数
		static std::vector<const char*> classText = {
			"NormalBillboard2D","SequenceBillboard2D","ClipBillboard2D"
		};
		static int classID = 0;
		static int currentItem = 0;

		// 変更されて渡される値は添え字
		ImGui::Combo("New Instance Type", &classID, classText.data(), static_cast<int>(classText.size()));
		if (ImGui::Button("Create")) { functions[classID](); }

		// インスタンス一覧
		if (!billboards_.list.empty()) {
			std::vector<const char*> itemText;
			for (IPlane* p : billboards_.list) {
				itemText.push_back(p->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			auto it = Utility::GetIteratorAtIndex(billboards_.list, currentItem);
			if (it != billboards_.list.end()) { (*it)->DebugGUI(); } // リスト内にあったなら呼び出す
		}
	}
}