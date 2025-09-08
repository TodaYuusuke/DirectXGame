#include "Collision2D.h"
#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

#include <typeinfo>

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Object::Collider2D;

Collision2D::Collision2D() {
	// デフォルトの形状にfollowをセット
	GetBasePtr(broad)->SetFollowPtr(&worldTF);
	CollisionManager::GetInstance()->SetPointer(this);
}

Collision2D::~Collision2D() {
	CollisionManager::GetInstance()->DeletePointer(this);
}

void Collision2D::Update() {
	ICollider* ptr = GetBasePtr(broad);
	
	// ブロードフェーズ更新
	ptr->Update();
	// ナローフェーズ更新
	for (ShapeVariant2D& n : narrows) {
		GetBasePtr(n)->Update();
	}

	// モートン序列番号を更新
	Rectangle2D bounding = ptr->GetBoundingRect();	// ブロードのAABBを取得
	// 点の場合の処理
	//if (ptr->GetShape() == Shape::Point) {
	//	mortonNumber = octree_->GetMortonNumber(min);	// 点だけ送る
	//}
	// それ以外の処理
	mortonNumber = quadTree_->GetMortonNumber(bounding);
	
	// ブロードにはモートン序列番号を渡しておく（Terrainとの当たり判定用）
	ptr->mortonNumber = mortonNumber;
}

void Collision2D::UnSetFollow() {
	worldTF.ClearParent();
}
void Collision2D::SetFollow(Object::TransformQuat * ptr) {
	worldTF.Parent(ptr);
}

void Collision2D::ApplyFixVector(const LWP::Math::Vector3& fixVector) {
	fixVector;
}

bool Collision2D::CheckMask(Collision2D* c) { return mask.CheckBelong(c->mask) && c->mask.CheckBelong(mask); }
void Collision2D::CheckCollision(Collision2D* c) {
	// お互いがアクティブかつマスクが成立していない -> 早期リターン
	if (!(isActive && c->isActive && (CheckMask(c)))) { return; }

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

void Collision2D::Hit(Collision2D* c) {
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
void Collision2D::NoHit(Collision2D* c) {
	int targetIndex = c->GetSerial();

	// 1以上なら前フレーム以前にヒットしているのでExitを呼び出す
	if (serialMap[targetIndex] >= 1) {
		exitLambda(c);
	}

	// ヒットフレーム数を0に
	serialMap[targetIndex] = 0;
}
Vector2 Collision2D::GetScreenPosition() {
	Vector3 w = worldTF.GetWorldPosition();
	return { w.x,w.y };
}

void Collision2D::DebugGUI() {
	// ワールドトランスフォーム
	worldTF.DebugGUI();
	
	// ブロードフェーズ
	if (ImGui::TreeNode("Broad")) {
		ImGui::Text("Shape Type : %s", Utility::TrimmingString(GetCurrentTypeName(broad), 31, 0).c_str());
		if (ImGui::BeginMenu("Change Shape Type")) {
			if (ImGui::MenuItem("Circle")) { SetBroadShape<Collider2D::Circle>(); }
			if (ImGui::MenuItem("Rectangle")) { SetBroadShape<Collider2D::Rectangle>(); }
			ImGui::EndMenu();
		}
		GetBasePtr(broad)->DebugGUI();
		ImGui::TreePop();
	}
	// ナローフェーズ
	if (!narrows.empty() && ImGui::TreeNode("Narrows")) {
		std::vector<const char*> itemText;
		static int currentItem = 0;
		for (ShapeVariant2D& v : narrows) {
			itemText.push_back(Utility::TrimmingString(typeid(v).name(), 48, 1).c_str());
		}
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
		GetBasePtr(narrows[currentItem])->DebugGUI();
		
		ImGui::TreePop();
	}
	mask.DebugGUI();

	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
	ImGui::Text(std::string("serialNumber : " + std::to_string(serialNum)).c_str());
	ImGui::Text(std::string("mortonNumber : " + std::to_string(mortonNumber)).c_str());
}

bool Collision2D::CheckBroadCollision(ShapeVariant2D& c, Math::Vector3* fixVec) {
	return std::visit([fixVec](auto& f, auto& t) {
		Collider2D::ICollider* a = &f;
		return a->CheckCollision(t, fixVec);
	}, broad, c);
}

ICollider* Collision2D::GetBasePtr(ShapeVariant2D& variant) {
	return std::visit([](auto& shape) -> Collider2D::ICollider* {
		return &shape;
	}, variant);
}
std::string Collision2D::GetCurrentTypeName(const ShapeVariant2D& variant) {
	return std::visit([](const auto& shape) {
		return std::string(typeid(shape).name());
	}, variant);
}

bool Collision2D::CheckNarrowsCollision(Collision2D* c, Vector3* fixVec) {
	// 相手にナローがない場合
	if (c->narrows.empty()) {
		// ナロー一つ一つと相手のブロードを比較
		for (ShapeVariant2D& narrow : narrows) {
			if (std::visit([fixVec](auto& f, auto& t) {
				Collider2D::ICollider* a = &f;
				return a->CheckCollision(t, fixVec);
				}, narrow, c->broad)) {
				return true;	// ヒットしていたのでループを終了
			}
		}
	}
	// 相手にナローがある場合
	else {
		// ナローと相手のナローを１つずつ比較
		for (ShapeVariant2D & f : narrows) {
			for (ShapeVariant2D& t : c->narrows) {
				if (std::visit([fixVec](auto& f, auto& t) {
					Collider2D::ICollider* a = &f;
					return a->CheckCollision(t, fixVec);
					}, f, t)) {
					return true;	// ヒットしていたのでループを終了
				}
			}
		}
	}

	return false;	// ヒットしていないのでfalseを返す
}

