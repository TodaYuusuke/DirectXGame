#include "Collider.h"
#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

#include <typeinfo>

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object::Collider;

Collider::Collider() {
	// デフォルトの形状にfollowをセット
	GetBasePtr(broad)->SetFollowPtr(&worldTF);
	System::engine->colliderManager_->SetPointer(this);
}

Collider::~Collider() {
	System::engine->colliderManager_->DeletePointer(this);
}

void Collider::Update() {
	// ブロードフェーズ更新
	GetBasePtr(broad)->Update();
	// ナローフェーズ更新
	for (ShapeVariant& n : narrows) {
		GetBasePtr(n)->Update();
	}
}

void Collider::SetFollowTarget(Object::TransformQuat* ptr) {
	followTF = ptr;	// ポインタを保持
	worldTF.Parent(ptr);
}

void Collider::ApplyFixVector(const LWP::Math::Vector3& fixVector) {
	// 追従対象がいるなら対象に適応
	if (followTF) { followTF->translation += fixVector;  }
	// いないなら自分に適応
	else { worldTF.translation += fixVector; }
}

void Collider::CheckCollision(Collider* c) {
	// 埋まっている場合の修正ベクトル
	Vector3 fixVec = { 0.0f,0.0f,0.0f };

	// １．ブロードフェーズチェック
	if (!CheckBroadCollision(c->broad, &fixVec)) {
		// Noヒット処理を行う
		NoHit(c);	// 自分の処理
		c->NoHit(this);	// 相手の処理
		return;	// ヒットしていないので早期リターン
	}

	// ２．ナローフェーズがある場合 -> チェック
	if (!narrows.empty()) {
		if (!CheckNarrowsCollision(c, &fixVec)) {
			// Noヒット処理を行う
			NoHit(c);	// 自分の処理
			c->NoHit(this);	// 相手の処理
			return;	// ヒットしていないならば処理を終了
		}
	}

	// ３．ヒット処理を行う
	Hit(c);	// 自分の処理
	c->Hit(this);	// 相手の処理

	// 4. 修正ベクトルを適応（isMoveの可否で決まる）

	// どちらもtrue
	if (isMove && c->isMove) {
		ApplyFixVector(fixVec / 2.0f);
		c->ApplyFixVector(fixVec / 2.0f);
	}
	// 自分だけtrue
	else if (isMove && !c->isMove) {
		ApplyFixVector(fixVec);
	}
	// 相手だけtrue
	else if (!isMove && c->isMove) {
		c->ApplyFixVector(fixVec);
	}
	// どちらもfalseなら何もしない
}

void Collider::Hit(Collider* c) {
	int targetIndex = c->GetSerial();

	if(serialMap[targetIndex] <= 0)	{
		enterLambda(c);	// 0なら前フレームヒットしていないのでEnter
	}
	else {
		stayLambda(c);	// そうでないなら前フレームヒットしているのでStay
	}
	
	// ヒットフレーム数+1
	serialMap[targetIndex]++;
}
void Collider::NoHit(Collider* c) {
	int targetIndex = c->GetSerial();

	// 1以上なら前フレーム以前にヒットしているのでExitを呼び出す
	if (serialMap[targetIndex] >= 1) {
		exitLambda(c);
	}

	// ヒットフレーム数を0に
	serialMap[targetIndex] = 0;
}

void Collider::DebugGUI() {
	// ワールドトランスフォーム
	worldTF.DebugGUI();
	
	// ブロードフェーズ
	if (ImGui::TreeNode("Broad")) {
		ImGui::Text("Shape Type : %s", Utility::TrimmingString(GetCurrentTypeName(broad), 29, 0).c_str());
		if (ImGui::BeginMenu("Change Shape Type")) {
			if (ImGui::MenuItem("Point")) { SetBroadShape(Point()); }
			if (ImGui::MenuItem("AABB")) { SetBroadShape(AABB()); }
			if (ImGui::MenuItem("Sphere")) { SetBroadShape(Sphere()); }
			if (ImGui::MenuItem("Capsule")) { SetBroadShape(Capsule()); }
			//if (ImGui::MenuItem("Mesh")) { SetBroadShape(Mesh()); }	// Meshの動的生成は厳しいので廃止
			ImGui::EndMenu();
		}
		GetBasePtr(broad)->DebugGUI();
		ImGui::TreePop();
	}
	// ナローフェーズ
	if (!narrows.empty() && ImGui::TreeNode("Narrows")) {
		std::vector<const char*> itemText;
		static int currentItem = 0;
		for (ShapeVariant& v : narrows) {
			itemText.push_back(Utility::TrimmingString(typeid(v).name(), 48, 1).c_str());
		}
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
		GetBasePtr(narrows[currentItem])->DebugGUI();
		
		ImGui::TreePop();
	}

	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
	ImGui::Text(std::string("serialNumber : " + std::to_string(serialNum)).c_str());
}

bool Collider::CheckBroadCollision(ShapeVariant& c, Math::Vector3* fixVec) {
	return std::visit([fixVec](auto& f, auto& t) {
		ICollisionShape* a = &f;
		return a->CheckCollision(t, fixVec);
	}, broad, c);
}

ICollisionShape* Collider::GetBasePtr(ShapeVariant& variant) {
	return std::visit([](auto& shape) -> ICollisionShape* {
		return &shape;
	}, variant);
}
std::string Collider::GetCurrentTypeName(const ShapeVariant& variant) {
	return std::visit([](const auto& shape) {
		return std::string(typeid(shape).name());
	}, variant);
}

bool Collider::CheckNarrowsCollision(Collider* c, Vector3* fixVec) {
	// 相手にナローがない場合
	if (c->narrows.empty()) {
		// ナロー一つ一つと相手のブロードを比較
		for (ShapeVariant& narrow : narrows) {
			if (std::visit([fixVec](auto& f, auto& t) {
				ICollisionShape* a = &f;
				return a->CheckCollision(t, fixVec);
				}, narrow, c->broad)) {
				return true;	// ヒットしていたのでループを終了
			}
		}
	}
	// 相手にナローがある場合
	else {
		// ナローと相手のナローを１つずつ比較
		for (ShapeVariant& f : narrows) {
			for (ShapeVariant& t : c->narrows) {
				if (std::visit([fixVec](auto& f, auto& t) {
					ICollisionShape* a = &f;
					return a->CheckCollision(t, fixVec);
					}, f, t)) {
					return true;	// ヒットしていたのでループを終了
				}
			}
		}
	}

	return false;	// ヒットしていないのでfalseを返す
}

