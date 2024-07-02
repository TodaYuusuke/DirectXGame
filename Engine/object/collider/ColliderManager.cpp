#include "ColliderManager.h"
#include <algorithm>

#include "utility/MyUtility.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object::Collider;

Manager::Manager() : indexManager_(lwpC::Collider::kMaxColliderAmount) {};

void Manager::Initialize() {
	for (Collider* c : debugPris) {
		delete c;
	}
	debugPris.clear();
	colliders_.list.clear();
}

void Manager::Update() {
	// 全体を更新
	for (Collider* c : colliders_.list) {
		c->Update();
	}

	// 当たり判定チェック
	for (int f = 0; f < colliders_.list.size(); f++) {
		// イテレーター取得
		std::list<Collider*>::iterator itrF = Utility::GetIteratorAtIndex<Collider*>(colliders_.list, f);
		for (int t = f + 1; t < colliders_.list.size() && (*itrF)->isActive; t++) {
			// イテレーター取得
			std::list<Collider*>::iterator itrT = Utility::GetIteratorAtIndex<Collider*>(colliders_.list, t);
			// isActiveがtrue　かつ　マスク処理が成立していてる場合
			if ((*itrT)->isActive && CheckMask((*itrF), (*itrT))) {
				// 当たり判定をチェックする
				(*itrF)->CheckCollision(*itrT);
			}
		}
	}
}

void Manager::SetPointer(Collider* ptr) {
	colliders_.SetPointer(ptr);
	// Indexを与える
	ptr->SetSerial(indexManager_.UseEmpty());
}
void Manager::DeletePointer(Collider* ptr) {
	colliders_.DeletePointer(ptr);
}

bool Manager::CheckMask(Collider* f, Collider* t) { return t->mask.CheckBelong(f->mask) || f->mask.CheckBelong(t->mask); }

void Manager::DebugGUI() {
	// ImGuiを表示
	if (ImGui::BeginTabItem("Collider")) {
		// 新しいインスタンスを作成
		if (ImGui::Button("Create New Instance")) {
			debugPris.push_back(new Collider());
		}

		// 形状一覧
		if (!colliders_.list.empty()) {
			std::vector<const char*> itemText;
			for (Collider* c : colliders_.list) {
				itemText.push_back(c->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			(*Utility::GetIteratorAtIndex<Collider*>(colliders_.list, currentItem))->DebugGUI();
		}
		ImGui::EndTabItem();
	}
}