#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "utility/MyUtility.h"

using namespace LWP::Primitive;

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
	// 更新
	for (IPrimitive* p : primitives_.list) {
		p->Update();
	}
}

void Manager::Draw(Base::RendererManager* manager) {
	// 描画
	for (IPrimitive* p : primitives_.list) {
		p->Draw(manager);
	}
}

// インスタンスのポインタをセット（ユーザー呼び出し不要）
void Manager::SetPointer(IPrimitive* ptr) {
	primitives_.SetPointer(ptr);
	ptr->name += std::to_string(primitiveCount_++);
}
// インスタンスのポインタを解放（ユーザー呼び出し不要）
void Manager::DeletePointer(IPrimitive* ptr) {
	primitives_.DeletePointer(ptr);
}


void Manager::DebugGUI() {
	// ImGuiを表示

	// 生成用の関数ポインタ
	static std::vector<std::function<void()>> functions = {
		[this]() { debugPris.push_back(new Billboard2D()); },
		[this]() { debugPris.push_back(new Billboard3D()); },
		[this]() { debugPris.push_back(new Surface()); },
		//[this]() { debugPris.push_back(new Sprite()); },
		[this]() { debugPris.push_back(new Triangle()); },
		[this]() { debugPris.push_back(new Capsule()); },
		[this]() { debugPris.push_back(new Cube()); },
		//[this]() { debugPris.push_back(new Mesh()); },
		[this]() { debugPris.push_back(new Sphere()); }
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"Billboard2D","Billboard3D","Surface",/*"Sprite",*/"Triangle", "Capsule", "Cube",/*"Mesh",*/ "Sphere"
	};

	if (ImGui::BeginTabItem("Primitive")) {
		// 変更されて渡される値は添え字
		ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
		if (ImGui::Button("Create")) { functions[selectedClass](); }

		// 形状一覧
		if (!primitives_.list.empty()) {
			std::vector<const char*> itemText;
			for (IPrimitive* p : primitives_.list) {
				itemText.push_back(p->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			(*Utility::GetIteratorAtIndex<IPrimitive*>(primitives_.list, currentItem))->DebugGUI();
		}
		ImGui::EndTabItem();
	}
}