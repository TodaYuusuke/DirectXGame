#include "ColliderManager.h"
#include <algorithm>

#include "utility/MyUtility.h"
#include <iostream>

#if DEMO
#include <component/Object.h>
#endif

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object::Collider;

void Manager::Initialize() {
	//for (ICollider* c : colliders_) {
	//	delete c;
	//}
	//colliders_.clear();
	colliderCount_ = 0;

	// 関数ポインタをセット
	checkCollisions_[0][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<AABB*>(c2)); };
	//checkCollisions_[0][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<OBB*>(c2)); };
	checkCollisions_[0][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<Sphere*>(c2)); };
	checkCollisions_[0][3] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<AABB*>(c1), dynamic_cast<Capsule*>(c2)); };
	//checkCollisions_[1][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<AABB*>(c2)); };
	//checkCollisions_[1][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<OBB*>(c2)); };
	//checkCollisions_[1][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<Sphere*>(c2));
	//checkCollisions_[1][3] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<OBB*>(c1), dynamic_cast<Capsule*>(c2));
	checkCollisions_[2][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<AABB*>(c2)); };
	//checkCollisions_[2][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<OBB*>(c2));
	checkCollisions_[2][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<Sphere*>(c2)); };
	checkCollisions_[2][3] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Sphere*>(c1), dynamic_cast<Capsule*>(c2)); };
	checkCollisions_[3][0] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Capsule*>(c1), dynamic_cast<AABB*>(c2)); };
	//checkCollisions_[3][1] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Capsule*>(c1), dynamic_cast<OBB*>(c2));
	checkCollisions_[3][2] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Capsule*>(c1), dynamic_cast<Sphere*>(c2)); };
	checkCollisions_[3][3] = [this](ICollider* c1, ICollider* c2) { return CheckCollision(dynamic_cast<Capsule*>(c1), dynamic_cast<Capsule*>(c2)); };
}


void Manager::Update() {
	// Debugビルド時のみImGuiを表示
#if DEMO
	// 生成用の関数ポインタ
	static std::vector<std::function<void()>> functions = {
		[this]() { debugPris.push_back(new AABB()); },
		/*[this]() { debugPris.push_back(new OBB()); },*/
		[this]() { debugPris.push_back(new Sphere()); },
		[this]() { debugPris.push_back(new Capsule()); },
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"AABB",/*"OBB",*/"Sphere","Capsule"
	};

	ImGui::Begin("LWP");

	if (ImGui::BeginTabBar("LWP")) {
		if (ImGui::BeginTabItem("Collider")) {
			// 変更されて渡される値は添え字
			ImGui::Combo("new Instance", &selectedClass, classText.data(), static_cast<int>(classText.size()));
			if (ImGui::Button("Create")) { 
				functions[selectedClass]();
			}

			// 形状一覧
			if (!colliders_.list.empty()) {
				std::vector<const char*> itemText;
				for (ICollider* c : colliders_.list) {
					itemText.push_back(c->name.c_str());
				}
				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
				(*Utility::GetIteratorAtIndex<ICollider*>(colliders_.list, currentItem))->DebugGUI();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

#endif

	// 全体を更新
	for (ICollider* c : colliders_.list) {
		c->Update();
	}

	// 当たり判定チェック
	for (int f = 0; f < colliders_.list.size(); f++) {
		// イテレーター取得
		std::list<ICollider*>::iterator itrF = Utility::GetIteratorAtIndex<ICollider*>(colliders_.list, f);
		for (int t = f + 1; t < colliders_.list.size() && (*itrF)->isActive; t++) {
			// イテレーター取得
			std::list<ICollider*>::iterator itrT = Utility::GetIteratorAtIndex<ICollider*>(colliders_.list, t);
			// isActiveがtrue　かつ　マスク処理が成立していて　かつ　ヒットしているかチェック
			if ((*itrT)->isActive && CheckMask((*itrF), (*itrT)) &&
				checkCollisions_[static_cast<int>((*itrF)->GetShape())][static_cast<int>((*itrT)->GetShape())]((*itrF), (*itrT))) {
				// ヒットしていた場合 -> ヒットフラグをtrueする
				(*itrF)->SetHit(true);
				(*itrT)->SetHit(true);
				// ラムダ実行
				(*itrF)->ExecuteLambda((*itrT));
				(*itrT)->ExecuteLambda((*itrF));
			}
		}

		// 一度もヒットしていないならばそれはそれで実行
		if (!(*itrF)->GetHit()) {
			(*itrF)->ExecuteLambda(nullptr);
		}
	}
}

void Manager::SetPointer(ICollider* ptr) {
	colliders_.SetPointer(ptr);
	ptr->name += std::to_string(colliderCount_++);
}
void Manager::DeletePointer(ICollider* ptr) {
	colliders_.DeletePointer(ptr);
}


bool Manager::CheckMask(ICollider* f, ICollider* t) { return t->mask.CheckBelong(f->mask) || f->mask.CheckBelong(t->mask); }

bool Manager::CheckCollision(AABB* c1, AABB* c2) {
	AABB_Data data1 = *c1;	// transformをかけたデータで計算する
	AABB_Data data2 = *c2;

	return (data1.min.x <= data2.max.x && data1.max.x >= data2.min.x) &&
		(data1.min.y <= data2.max.y && data1.max.y >= data2.min.y) &&
		(data1.min.z <= data2.max.z && data1.max.z >= data2.min.z);
}
//bool Manager::CheckCollision(AABB* f, OBB* t) {
//	f;	t; return false;
//}
bool Manager::CheckCollision(AABB* c1, Sphere* c2) {
	AABB_Data aabb = *c1;
	Sphere_Data sphere = *c2;

	// 最近接点
	Vector3 closestPoint = {
		std::clamp(sphere.position.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.position.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.position.z, aabb.min.z, aabb.max.z),
	};

	float dist = (closestPoint - sphere.position).Length();
	return dist <= sphere.radius;
}
bool Manager::CheckCollision(AABB* c1, Capsule* c2) {
	AABB_Data aabb = *c1;	// transformをかけたデータで計算する
	Capsule_Data capsule = *c2;

	Vector3 d = aabb.center - capsule.start;
	Vector3 ba = capsule.end - capsule.start;
	// カプセルのベクトルの長さ
	float length = ba.Length();
	// 正規化
	Vector3 e = ba.Normalize();
	// 内積
	float dot = Vector3::Dot(d, e);

	float t = dot / length;
	t = std::clamp<float>(t, 0.0f, 1.0f);
	// 線形補間
	Vector3 f;
	f.x = (1.0f - t) * capsule.start.x + t * capsule.end.x;
	f.y = (1.0f - t) * capsule.start.y + t * capsule.end.y;
	f.z = (1.0f - t) * capsule.start.z + t * capsule.end.z;

	// 最近接点
	Vector3 closestPoint = {
		std::clamp(f.x, aabb.min.x, aabb.max.x),
		std::clamp(f.y, aabb.min.y, aabb.max.y),
		std::clamp(f.z, aabb.min.z, aabb.max.z),
	};

	float dist = (closestPoint - f).Length();
	return dist <= capsule.radius;
}
//bool Manager::CheckCollision(OBB* f, OBB* t) {
//	f;	t; return false;
//}
//bool Manager::CheckCollision(OBB* f, Sphere* t) {
//	f;	t; return false;
//}
bool Manager::CheckCollision(Sphere* c1, Sphere* c2) {
	Sphere_Data data1 = *c1;	// transformをかけたデータで計算する
	Sphere_Data data2 = *c2;

	// 二つの球体の中心点間の距離を求める
	Vector3 dist = data1.position - data2.position;
	// 半径の合計よりも短ければ衝突
	return dist.Length() <= (data1.radius + data2.radius);
}
bool Manager::CheckCollision(Sphere* c1, Capsule* c2) {
	Sphere_Data sphere = *c1;
	Capsule_Data capsule = *c2;

	Vector3 d = sphere.position - capsule.start;
	Vector3 ba =capsule.end - capsule.start;
	// カプセルのベクトルの長さ
	float length = ba.Length();
	// 正規化
	Vector3 e = ba.Normalize();
	// 内積
	float dot = Vector3::Dot(d, e);

	float t = dot / length;
	t = std::clamp<float>(t, 0.0f, 1.0f);
	// 線形補間
	Vector3 f;
	f = (1.0f - t) * capsule.start + t * capsule.end;

	Vector3 c = sphere.position - f;
	// 距離
	float distance = c.Length();

	// 当たっているかを判定
	return distance < sphere.radius + capsule.radius;
}
bool Manager::CheckCollision(Capsule* c1, Capsule* c2) {
	Utility::Log("Error!! Capsule * Capsule Collision is Unimplemented");
	c1;	c2; return false;
}