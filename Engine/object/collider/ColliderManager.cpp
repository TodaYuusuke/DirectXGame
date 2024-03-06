#include "ColliderManager.h"

#if DEMO
#include <component/Object.h>
#endif

using namespace LWP;
using namespace LWP::Object::Collider;

void Manager::Initialize() {
	for (ICollider* c : colliders_) {
		delete c;
	}
	colliders_.clear();

	// 関数ポインタをセット
	checkCollisions_[0][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<AABB*>(c2)); };
	//checkCollisions_[0][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<OBB*>(c2));
	//checkCollisions_[0][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<Sphere*>(c2));
	//checkCollisions_[1][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<AABB*>(c2));
	//checkCollisions_[1][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<OBB*>(c2));
	//checkCollisions_[1][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<Sphere*>(c2));
	//checkCollisions_[2][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<AABB*>(c2));
	//checkCollisions_[2][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<OBB*>(c2));
	//checkCollisions_[2][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<Sphere*>(c2));
}


void Manager::Update() {
	// Debugビルド時のみImGuiを表示
#if DEMO
	// 生成用の関数ポインタ
	static std::vector<std::function<ICollider*()>> functions = {
		&LWP::Object::Collider::CreateInstance<AABB>,
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"AABB"
	};

	ImGui::Begin("LWP", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTabBar("LWP")) {
		if (ImGui::BeginTabItem("Collider")) {
			// 変更されて渡される値は添え字
			ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
			if (ImGui::Button("Create")) { 
				functions[selectedClass]();
			}

			// 形状一覧
			if (!colliders_.empty()) {
				std::vector<const char*> itemText;
				for (ICollider* c : colliders_) {
					itemText.push_back(c->name.c_str());
				}
				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
				colliders_[currentItem]->DebugGUI();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	// デバッグしやすいようにワイヤーフレームを描画
	for (ICollider* c : colliders_) {
		c->ShowWireFrame(LWP::System::engine->directXCommon_->GetCommandManager());
	}
#endif

	// 当たり判定チェック
	for (int f = 0; f < colliders_.size(); f++) {
		for (int t = f + 1; t < colliders_.size(); t++) {
			checkCollisions_
				[static_cast<int>(colliders_[f]->GetShape())]
				[static_cast<int>(colliders_[t]->GetShape())]
				(colliders_[f], colliders_[t]);
		}
	}
}



bool Manager::CheckCollision(AABB* f, AABB* t) {
	if ((f->min.x <= t->max.x && f->max.x >= t->min.x) &&
		(f->min.y <= t->max.y && f->max.y >= t->min.y) &&
		(f->min.z <= t->max.z && f->max.z >= t->min.z)) {
		
		f->hitting = true;
		t->hitting = true;
		return true;	// ヒットしているのでtrue
	}

	f->hitting = false;
	t->hitting = false;
	return false;	// 単純な当たり判定を返す
}
//bool Manager::CheckCollision(AABB* f, OBB* t) {
//	f;	t; return false;
//}
//bool Manager::CheckCollision(AABB* f, Sphere* t) {
//	f;	t; return false;
//}
//bool Manager::CheckCollision(OBB* f, OBB* t) {
//	f;	t; return false;
//}
//bool Manager::CheckCollision(OBB* f, Sphere* t) {
//	f;	t; return false;
//}
//bool Manager::CheckCollision(Sphere* f, Sphere* t) {
//	f;	t; return false;
//}