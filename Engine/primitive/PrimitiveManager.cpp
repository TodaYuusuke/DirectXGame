#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "object/ObjectManager.h"
#include "Config.h"
#include "utility/MyUtility.h"

using namespace LWP::Math;

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

	Manager::Manager() :
		sorted_(lwpC::Rendering::Primitive::Billboaed::kMaxCount) {
		sorted_.Init();
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
		sorted_.Reset();

		int i = 0;
		std::vector<BillboardIndex> indexes;
		Math::Vector3 cameraPos = Object::Manager::GetInstance()->GetMainCamera()->worldTF.GetWorldPosition();

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
		// 更新処理（ビルボード）
		for (IPlane* p : billboards3D_.list) {
			// primitiveを更新
			p->Update();

			// バッファーにデータをセット
			if (p->isActive) {
				for (const Vertex& v : p->vertices) { billboard3DBuffers_.vertices.Add(v); }
				billboard3DBuffers_.materials.Add(p->material);
				billboard3DBuffers_.wtf.Add(p->worldTF);
				billboard3DBuffers_.count++;

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
		for (BillboardIndex& b : indexes) { sorted_.Add(b.index); }
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
	void Manager::SetBillboard2DPtr(IPlane* ptr) {
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