#include "CollisionManager.h"
#include <algorithm>

#include "utility/MyUtility.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Object::Collider;

CollisionManager::CollisionManager() : indexManager_(lwpC::Collider::kMaxColliderAmount) {};

void CollisionManager::Initialize() {
	for (Collision* c : debugCols) {
		delete c;
	}
	debugCols.clear();
	collisions_.list.clear();
}

void CollisionManager::Update() {
	// 全体を更新
	for (Collision* c : collisions_.list) {
		c->Update();
	}

	// 当たり判定チェック
	for (int f = 0; f < collisions_.list.size(); f++) {
		// イテレーター取得
		std::list<Collision*>::iterator itrF = Utility::GetIteratorAtIndex<Collision*>(collisions_.list, f);
		for (int t = f + 1; t < collisions_.list.size() && (*itrF)->isActive; t++) {
			// イテレーター取得
			std::list<Collision*>::iterator itrT = Utility::GetIteratorAtIndex<Collision*>(collisions_.list, t);
			// isActiveがtrue　かつ　マスク処理が成立していてる場合
			if ((*itrT)->isActive && CheckMask((*itrF), (*itrT))) {
				// 当たり判定をチェックする
				(*itrF)->CheckCollision(*itrT);
			}
		}
	}
}

void CollisionManager::SetPointer(Collision* ptr) {
	collisions_.SetPointer(ptr);
	// Indexを与える
	ptr->SetSerial(indexManager_.UseEmpty());
}
void CollisionManager::DeletePointer(Collision* ptr) {
	collisions_.DeletePointer(ptr);
}

bool CollisionManager::CheckMask(Collision* f, Collision* t) { return t->mask.CheckBelong(f->mask) || f->mask.CheckBelong(t->mask); }

void CollisionManager::DebugGUI() {
	// ImGuiを表示
	if (ImGui::BeginTabItem("Collider")) {
		// 新しいインスタンスを作成
		if (ImGui::Button("Create New Instance")) {
			debugCols.push_back(new Collision());
		}

		// 形状一覧
		if (!collisions_.list.empty()) {
			std::vector<const char*> itemText;
			for (Collision* c : collisions_.list) {
				itemText.push_back(c->name.c_str());
			}
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			(*Utility::GetIteratorAtIndex<Collision*>(collisions_.list, currentItem))->DebugGUI();
		}
		ImGui::EndTabItem();
	}
}