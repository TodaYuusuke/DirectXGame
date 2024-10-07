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
	// モートン順に並べたmapを作る
	std::map<int, std::vector<Collision*>> map;
	std::list<Collision*> stackList;	// 上位空間のコリジョンをまとめたデータ

	// 全体を更新
	for (Collision* c : collisions_.list) {
		c->Update();
		map[c->GetMortonNumber()].push_back(c);	// mapに配置
	}

	int currentNum = 0;			// 現在検証中のモートン序列番号
	uint32_t currentSpaceLevel = 0;	// 現在検証中の空間レベル
	
	// 最低1回はループしてもらう
	do {
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

		// スタックリストにこの空間のデータを追加
		for (int i = 0; i < size; i++) {
			stackList.push_back(map[currentNum][i]);
		}

		// -- 次の空間検索開始 -- //

		// 最下層の空間でないならば -> 子空間に移動
		if (octree_.divisionLevel > currentSpaceLevel) {
			currentSpaceLevel++;	// 空間レベル増加
			currentNum = (currentNum << 3) + 1;	// 子空間の先頭へ
		}
		// 最下層の空間の場合
		else {
			// サイズを超過していない番号になるまでループ
			bool completeFlag = false;

			while (!completeFlag) {
				// 成功したと仮定
				completeFlag = true;

				// 同じ空間内の次の番号にパス
				currentNum++;
				
				// 線形でない場合の自分の空間番号を求める
				int normalNum = currentNum - octree_.GetSpaceLevelObjectsSum(currentSpaceLevel);	// これで求まる
				// 同空間内のサイズを超過した場合 -> １つ上の空間に戻る
				if (normalNum >= 8) {
					// スタックから自分のデータを削除
					for (int i = 0; i < static_cast<int>(map[--currentNum].size()); i++) {
						stackList.pop_back();
					}
					currentSpaceLevel--;	// 空間レベル-1
					currentNum = (currentNum - 1) >> 3;	// 親空間へ戻る

					// ルート空間なら終了
					if (currentSpaceLevel == 0) { break; }

					// 完了していないのでフラグをfalseに
					completeFlag = false;
				}
			}
		}
	} while (currentNum != 0);	// ルート空間に戻ってきたなら終了
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
			ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
			(*Utility::GetIteratorAtIndex<Collision*>(collisions_.list, currentItem))->DebugGUI();
		}
		ImGui::EndTabItem();
	}
}