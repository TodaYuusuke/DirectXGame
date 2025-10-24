#include "CollisionManager.h"
#include <algorithm>

#include "utility/functions/ArrayFunctions.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Object::Collider;

CollisionManager::CollisionManager() : indexManager_(lwpC::Collider::kMaxColliderAmount) {};

void CollisionManager::Init() {
	for (Collision* c : debugCols) { delete c; }
	for (Collision2D* c : debugCols2D) { delete c; }
	
	debugCols.clear();
	debugCols2D.clear();
	collisions_.list.clear();
}

void CollisionManager::Update() {
	octree_.CheckAllCollisions(&collisions_.list);	// 当たり判定をチェックしてもらう
	quadTree_.CheckAllCollisions(&collisions2D_.list);
}

void CollisionManager::SetPointer(Collision* ptr) {
	collisions_.SetPtr(ptr);
	ptr->SetSerial(indexManager_.UseEmpty());	// Indexを与える
	ptr->SetOctree(&octree_);	// オクトツリーを渡す
}
void CollisionManager::SetPointer(Collision2D* ptr) {
	collisions2D_.SetPtr(ptr);
	ptr->SetSerial(indexManager_.UseEmpty());	// Indexを与える
	ptr->SetQuadTree(&quadTree_);	// オクトツリーを渡す
}
void CollisionManager::DeletePointer(Collision* ptr) {
	collisions_.DeletePtr(ptr);
}
void CollisionManager::DeletePointer(Collision2D* ptr) {
	collisions2D_.DeletePtr(ptr);
}

void CollisionManager::DebugGUI() {
	// ImGuiを表示
	if (ImGui::BeginTabItem("Collider")) {
		if (ImGui::TreeNode("3D")) {
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
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("2D")) {
			// 4分木のデバッグ表示
			if (ImGui::TreeNode("QuadTree Parameter")) {
				quadTree_.DebugGUI();
				ImGui::TreePop();
			}

			// ここから下はインスタンス管理
			ImGui::Text("--- Instance Manager ---");

			// 新しいインスタンスを作成
			if (ImGui::Button("Create New Instance")) {
				debugCols2D.push_back(new Collision2D());
			}
			// 形状一覧
			if (!collisions2D_.list.empty()) {
				std::vector<const char*> itemText;
				for (Collision2D* c : collisions2D_.list) {
					itemText.push_back(c->name.c_str());
				}
				// 現在のアイテム番号が場外にならないように修正
				if (currentItem >= static_cast<int>(collisions2D_.list.size())) { currentItem = static_cast<int>(collisions2D_.list.size()) - 1; }

				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
				(*Utility::GetIteratorAtIndex<Collision2D*>(collisions2D_.list, currentItem))->DebugGUI();
			}
			ImGui::TreePop();
		}
		ImGui::EndTabItem();
	}
}