#include "UtilityManager.h"

namespace LWP::Utility {
	Manager::Manager() { Init(); }

	void Manager::Init() {
		count_ = 0;

		for (IUtility* p : debugInstance_) {
			delete p;
		}
		debugInstance_.clear();
	}

	void Manager::Update() {
		// 全更新処理
		for (IUtility* p : instances_.list) { p->Update(); }
	}

	void Manager::DebugGUI() {
		if (ImGui::BeginTabItem("Utility")) {
			ImGui::Text("---------- Create New Instance ----------");

			// 指定の種類のPrimitiveを絞り込む選択肢
			static std::vector<const char*> filterText = {
				"All","Timer"
			};
			static int filterID = 0;
			// 選択肢の変数
			static std::vector<const char*> typeText = {
				"Timer",
			};
			static int typeID = 0;

			// 新規生成用のインスタンスを作成する関数のリスト
			static std::vector<std::function<void()>> functions = {
				[this]() { debugInstance_.push_back(new Timer()); },
			};
			static int currentItem = 0;

			// インスタンス生成用のUI
			if (ImGui::BeginTable("ComboTable", 1, ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableSetupColumn("Type Select", ImGuiTableColumnFlags_WidthStretch);   // ラベル列

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Type Select");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(-FLT_MIN); // セルの左端から右端まで広げる
				ImGui::Combo("##TypeCombo", &typeID, typeText.data(), static_cast<int>(typeText.size()));

				ImGui::EndTable();
			}
			if (ImGui::Button("Create")) { functions[typeID](); }
			ImGui::Text("---------- Instance List ----------");
			ImGui::Combo("Filter by Type", &filterID, filterText.data(), static_cast<int>(filterText.size()));
			// インスタンス一覧
			if (!instances_.list.empty()) {
				std::vector<const char*> itemText;
				std::vector<IUtility*> ptr;
				for (IUtility* p : instances_.list) {
					if (filterID != 0) {	// All以外の時
						// 選択されたタイプ以外は表示しない
						if (p->GetType() != static_cast<IUtility::Type>(filterID - 1)) { continue; }
					}
					itemText.push_back(p->name.c_str());
					ptr.push_back(p);
				}
				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

				if (!ptr.empty() && 0 <= currentItem && currentItem < ptr.size()) {
					ptr[currentItem]->DebugGUI();
				}
				else {
					ImGui::Text("No Primitive selected.");
				}
			}

			ImGui::EndTabItem();
		}
	}
}