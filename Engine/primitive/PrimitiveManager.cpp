#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "utility/MyUtility.h"

using namespace LWP::Primitive;

void Manager::Initialize() {
	//for (IPrimitive* p : primitives_.list) {
	//	delete p;
	//}
	//primitives_.list.clear();
	primitiveCountMap_.clear();
}

void Manager::Update() {
	// Debugビルド時のみImGuiを表示
#if DEMO
	// 生成用の関数ポインタ
	static std::vector<std::function<void()>> functions = {
		[this]() { debugPris.push_back(Surface()); },
		[this]() { debugPris.push_back(Sprite()); },
		[this]() { debugPris.push_back(Triangle()); },
		[this]() { debugPris.push_back(Capsule()); },
		[this]() { debugPris.push_back(Cube()); },
		//&[this]() { debugPris.push_back(Mesh()); },
		[this]() { debugPris.push_back(Sphere()); }
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"Surface","Sprite","Triangle", "Capsule", "Cube", "Sphere"
	};

	ImGui::Begin("LWP", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTabBar("LWP")) {
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
		ImGui::EndTabBar();
	}
	ImGui::End();
#endif

	// 更新
	for (IPrimitive* p : primitives_.list) {
		p->Update();
	}
}

void Manager::Draw(Base::CommandManager* manager) {
	// 描画
	for (IPrimitive* p : primitives_.list) {
		p->Draw(manager);
	}
}

// インスタンスのポインタをセット（ユーザー呼び出し不要）
void Manager::SetPointer(IPrimitive* ptr) {
	primitives_.SetPointer(ptr);

	// カウントのマップから数を測定し、デフォルトの名前を登録
	if (!primitiveCountMap_.count(ptr->name)) {
		// 存在しない場合のみ0で初期化
		primitiveCountMap_[ptr->name] = 0;
	}
	ptr->name += std::to_string(primitiveCountMap_[ptr->name]++);
}
// インスタンスのポインタを解放（ユーザー呼び出し不要）
void Manager::DeletePointer(IPrimitive* ptr) {
	primitives_.DeletePointer(ptr);
}