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

void CollisionManager::Init() {
	for (Collision* c : debugCols) {
		delete c;
	}
	debugCols.clear();
	collisions_.list.clear();
}

void CollisionManager::Update() {
	// モートン順に並べたmapを作る
	std::map<int, std::vector<Collision*>> map;
	std::list<Collision*> stackList;	// 上位空間のコリジョンをまとめたデータ
	int maxResolution = octree_.GetSpaceLevelObjectsSum(octree_.divisionLevel + 1);	// 空間の最大分割数

	// 全体を更新
	for (Collision* c : collisions_.list) {
		c->Update();
		map[c->GetMortonNumber()].push_back(c);	// mapに配置
	}

	int currentNum = 0;			// 現在検証中のモートン序列番号
	
	while (true) {
		// 現在の空間に存在するコリジョンの数
		int size = static_cast<int>(map[currentNum].size());
		// 現在の空間内に登録されているオブジェクト同士を衝突
		for (int i = 0; i < size; i++) {
			for (int j = i + 1; j < size; j++) {
				map[currentNum][i]->CheckCollision(map[currentNum][j]);	// 今のコリジョン × 次のコリジョン
			}

			// スタック（リスト）に登録されているコリジョンとも衝突判定
			for (std::list<Collision*>::iterator it = stackList.begin(); it != stackList.end(); it++) {
				map[currentNum][i]->CheckCollision(*it);
			}
		}

		// -- 次の空間検索開始 -- //
		
		// 次の小空間が8分木分割数を超えていなければ移動
		if ((currentNum << 3) + 1 < maxResolution) {
			// スタックリストにこの空間のデータを追加
			for (int i = 0; i < size; i++) {
				stackList.push_back(map[currentNum][i]);
			}
			currentNum = (currentNum << 3) + 1;
			// 最初に戻る
			continue;
		}
		// そうでない場合は次のモートン番号に移動する
		else if (currentNum % 8 != 0) {
			currentNum++;
			// 最初に戻る
			continue;
		}

		// 下の空間に所属する小空間をすべて検証し終わった場合

		// １つ上の空間に戻る
		do {
			currentNum = (currentNum - 1) >> 3;
			// currentNumが実数値の場合 -> スタックからオブジェクトを外す
			if (currentNum >= 0) {
				size_t PopNum = map[currentNum].size();
				for (int i = 0; i < PopNum; i++) {
					stackList.pop_back();
				}
			}
			// 戻った空間がその空間の最後の数値の場合 -> もう一度戻る
		} while (currentNum % 8 == 0);

		// 次のモートン番号へ進む
		currentNum++;

		// 戻った空間が場外（-1）だった場合 -> ループ終了
		if (currentNum == 0) {
			break;
		}
	}

	// 全て終わった後にmapとlistをクリア
	map.clear();
	stackList.clear();
}

void CollisionManager::SetPointer(Collision* ptr) {
	collisions_.SetPointer(ptr);
	ptr->SetSerial(indexManager_.UseEmpty());	// Indexを与える
	ptr->SetOctree(&octree_);	// オクトツリーを渡す
}
void CollisionManager::DeletePointer(Collision* ptr) {
	collisions_.DeletePointer(ptr);
}

void CollisionManager::DebugGUI() {
	// ImGuiを表示
	if (ImGui::BeginTabItem("Collider")) {
		// 8分木のデバッグ表示
		if (ImGui::TreeNode("Octree Parameter")) {
			octree_.DebugGUI();
			ImGui::TreePop();
		}

		// ここから下はインスタンス管理
		ImGui::Text("--- Instance Manager ---");

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
			// 現在のアイテム番号が場外にならないように修正
			if (currentItem >= static_cast<int>(collisions_.list.size())) { currentItem = static_cast<int>(collisions_.list.size()) - 1; }

			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			(*Utility::GetIteratorAtIndex<Collision*>(collisions_.list, currentItem))->DebugGUI();
		}
		ImGui::EndTabItem();
	}
}