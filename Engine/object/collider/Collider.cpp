#include "Collider.h"
#include "base/ImGuiManager.h"
#include "component/System.h"

using namespace LWP;
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
	// ブロードフェーズ
	GetBasePtr(broad)->Update();
	// ナローフェーズ
	for (ShapeVariant& n : narrows) {
		GetBasePtr(n)->Update();
	}
}

void Collider::CheckCollision(Collider* c) {

	// １．ブロードフェーズチェック
	if (!CheckBroadCollision(c->broad)) {
		return;	// ヒットしていないので早期リターン
	}

	// ２．ナローフェーズがある場合 -> チェック
	if (!narrows.empty()) {
		if (!CheckNarrowsCollision(c)) {
			return;	// ヒットしていないならば処理を終了
		}
	}

	// ３．ヒット処理を行う
	Hit(c->GetSerial());	// 自分の処理
	c->Hit(GetSerial());	// 相手の処理
}

void Collider::Hit(int targetIndex) {
	targetIndex;
}

void Collider::DebugGUI() {
	// ワールドトランスフォーム
	worldTF.DebugGUI();
	
	// ブロードフェーズ
	if (ImGui::TreeNode("Broad")) {
		GetBasePtr(broad)->DebugGUI();
		ImGui::TreePop();
	}
	// ナローフェーズ
	//ImGui::Text("Narrows");
	//	for (ShapeVariant& v : narrows) {
	//		std::visit([](auto& f) {
	//			((ICollisionShape*)(&f))->DebugGUI();
	//		}, v);
	//	}

	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
}

bool Collider::CheckBroadCollision(ShapeVariant& c) {
	return std::visit([](auto& f, auto& t) {
		ICollisionShape* a = &f;
		return a->CheckCollision(t);
	}, broad, c);
}

bool Collider::CheckNarrowsCollision(Collider* c) {
	// 相手にナローがない場合
	if (c - narrows.empty()) {
		// ナロー一つ一つと相手のブロードを比較
		for (ShapeVariant& narrow : narrows) {
			if (std::visit([](auto& f, auto& t) {
				ICollisionShape* a = &f;
				return a->CheckCollision(t);
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
				if (std::visit([](auto& f, auto& t) {
					ICollisionShape* a = &f;
					return a->CheckCollision(t);
					}, f, t)) {
					return true;	// ヒットしていたのでループを終了
				}
			}
		}
	}

	return false;	// ヒットしていないのでfalseを返す
}

