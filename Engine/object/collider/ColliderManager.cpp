#include "ColliderManager.h"
#include <algorithm>

#if DEMO
#include <component/Object.h>
#endif

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object::Collider;

void Manager::Initialize() {
	for (ICollider* c : colliders_) {
		delete c;
	}
	colliders_.clear();

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
	static std::vector<std::function<ICollider*()>> functions = {
		&LWP::Object::Collider::CreateInstance<AABB>,
		//&LWP::Object::Collider::CreateInstance<OBB>,
		&LWP::Object::Collider::CreateInstance<Sphere>,
		&LWP::Object::Collider::CreateInstance<Capsule>,
	};
	// 選択肢の変数
	static std::vector<const char*> classText = {
		"AABB",/*"OBB",*/"Sphere","Capsule"
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

#endif

	// 全体を更新
	for (ICollider* c : colliders_) {
		c->Update();
		// デバッグしやすいようにワイヤーフレームを描画
		#if DEMO
		c->ShowWireFrame();
		#endif
	}

	// 当たり判定チェック
	for (int f = 0; f < colliders_.size(); f++) {
		for (int t = f + 1; t < colliders_.size() && colliders_[f]->isActive; t++) {
			// isActiveがtrue　かつ　マスク処理が成立していて　かつ　ヒットしているかチェック
			if (colliders_[t]->isActive && CheckMask(colliders_[f], colliders_[t]) &&
				checkCollisions_[static_cast<int>(colliders_[f]->GetShape())][static_cast<int>(colliders_[t]->GetShape())](colliders_[f], colliders_[t])) {
				// ヒットしていた場合 -> ヒットフラグをtrueする
				colliders_[f]->SetHit(true);
				colliders_[t]->SetHit(true);
				// ラムダ実行
				colliders_[f]->ExecuteLambda(colliders_[t]);
				colliders_[t]->ExecuteLambda(colliders_[f]);
			}
		}

		// 一度もヒットしていないならばそれはそれで実行
		if (!colliders_[f]->GetHit()) {
			colliders_[f]->ExecuteLambda(nullptr);
		}
	}
}


bool Manager::CheckMask(ICollider* f, ICollider* t) { return t->mask.CheckBelong(f->mask) || f->mask.CheckBelong(t->mask); }

bool Manager::CheckCollision(AABB* c1, AABB* c2) {
	AABB_Data data1 = *c1;	// transformをかけたデータで計算する
	AABB_Data data2 = *c2;

	if ((data1.min.x <= data2.max.x && data1.max.x >= data2.min.x) &&
		(data1.min.y <= data2.max.y && data1.max.y >= data2.min.y) &&
		(data1.min.z <= data2.max.z && data1.max.z >= data2.min.z)) {
		return true;	// ヒットしているのでtrue
	}
	return false;	// 単純な当たり判定を返す
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
	if (dist <= sphere.radius) {
		return true;
	}
	return false;
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
	if (t > 1) {
		t = 1;
	}
	else if (t < 0) {
		t = 0;
	}
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
	if (dist <= capsule.radius) {
		return true;
	}
	return false;
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
	if (dist.Length() <= data1.radius + data2.radius) {
		// 当たった処理
		return true;
	}

	return false;
}
bool Manager::CheckCollision(Sphere* c1, Capsule* c2) {
	c1;	c2; return false;
}
bool Manager::CheckCollision(Capsule* c1, Capsule* c2) {
	c1;	c2; return false;
}