#include "Collision.h"
#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include "component/System.h"

#include <typeinfo>

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Object::Collider;

void Collision::Mask::DebugGUI() {
	// 所属フラグかヒットフラグかを選ぶ
	static int mode = 0;
	// 文字列（ほんとはConfigから名前を取得したい）
	static std::string str[] = {
		"Player",
		"Enemy",
		"Bullet",
		"Ground",
		"Particle",
		"Layer5",
		"Layer6",
		"Layer7",
		"Layer8",
		"Layer9",
		"Layer10",
		"Layer11",
		"Layer12",
		"Layer13",
		"FieldObject",
		"Terrain",
	};

	if (ImGui::TreeNode("Mask")) {
		// どちらのフラグを設定するか
		ImGui::RadioButton("Belong", &mode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Hit", &mode, 1);
		// 選択された方を参照に
		uint32_t* flag = nullptr;
		if (mode == 0) { flag = &belongFrag; }
		else { flag = &hitFrag; }

		uint32_t result = 0;
		for (int i = 0; i < 16; i++) {
			bool b = *flag & (0b1 << i);
			ImGui::Checkbox(str[i].c_str(), &b);
			result += (b << i);
		}
		*flag = result;

		// 一括変更ボタン
		if (ImGui::Button("ALL false")) {
			*flag = lwpC::Collider::MaskLayer::None;
		}
		if (ImGui::Button("ALL true")) {
			*flag = lwpC::Collider::MaskLayer::ALL;
		}
		ImGui::TreePop();
	}
}

Collision::Collision() {
	// デフォルトの形状にfollowをセット
	GetBasePtr(broad)->SetFollowPtr(&worldTF);
	System::engine->collisionManager_->SetPointer(this);
}

Collision::~Collision() {
	System::engine->collisionManager_->DeletePointer(this);
}

void Collision::Update() {
	ICollider* ptr = GetBasePtr(broad);
	
	// ブロードフェーズ更新
	ptr->Update();
	// ナローフェーズ更新
	for (ShapeVariant& n : narrows) {
		GetBasePtr(n)->Update();
	}

	// モートン序列番号を更新
	Vector3 min = { 0.0f,0.0f, 0.0f };
	Vector3 max = { 0.0f,0.0f, 0.0f };
	ptr->GetBoundingAABB(&min, &max);	// ブロードのAABBを取得
	// 点の場合の処理
	if (ptr->GetShape() == Shape::Point) {
		mortonNumber = octree_->GetMortonNumber(min);	// 点だけ送る
	}
	// それ以外の処理
	else {
		mortonNumber = octree_->GetMortonNumber(min, max);
	}
	// ブロードにはモートン序列番号を渡しておく（Terrainとの当たり判定用）
	ptr->mortonNumber = mortonNumber;
}

void Collision::SetFollowTarget(Object::TransformQuat* ptr) {
	followTF = ptr;	// ポインタを保持
	worldTF.Parent(ptr);
}

void Collision::ApplyFixVector(const LWP::Math::Vector3& fixVector) {
	// 追従対象がいるなら対象に適応
	if (followTF) { followTF->translation += fixVector;  }
	// いないなら自分に適応
	else { worldTF.translation += fixVector; }
}

bool Collision::CheckMask(Collision* c) { return mask.CheckBelong(c->mask) && c->mask.CheckBelong(mask); }
void Collision::CheckCollision(Collision* c) {
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

void Collision::Hit(Collision* c) {
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
void Collision::NoHit(Collision* c) {
	int targetIndex = c->GetSerial();

	// 1以上なら前フレーム以前にヒットしているのでExitを呼び出す
	if (serialMap[targetIndex] >= 1) {
		exitLambda(c);
	}

	// ヒットフレーム数を0に
	serialMap[targetIndex] = 0;
}

void Collision::DebugGUI() {
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
			//if (ImGui::MenuItem("Terrain")) { SetBroadShape(Terrain()); }	// Terrainの動的生成は厳しいので廃止
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
	mask.DebugGUI();

	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
	ImGui::Text(std::string("serialNumber : " + std::to_string(serialNum)).c_str());
	ImGui::Text(std::string("mortonNumber : " + std::to_string(mortonNumber)).c_str());
}

bool Collision::CheckBroadCollision(ShapeVariant& c, Math::Vector3* fixVec) {
	return std::visit([fixVec](auto& f, auto& t) {
		ICollider* a = &f;
		return a->CheckCollision(t, fixVec);
	}, broad, c);
}

ICollider* Collision::GetBasePtr(ShapeVariant& variant) {
	return std::visit([](auto& shape) -> ICollider* {
		return &shape;
	}, variant);
}
std::string Collision::GetCurrentTypeName(const ShapeVariant& variant) {
	return std::visit([](const auto& shape) {
		return std::string(typeid(shape).name());
	}, variant);
}

bool Collision::CheckNarrowsCollision(Collision* c, Vector3* fixVec) {
	// 相手にナローがない場合
	if (c->narrows.empty()) {
		// ナロー一つ一つと相手のブロードを比較
		for (ShapeVariant& narrow : narrows) {
			if (std::visit([fixVec](auto& f, auto& t) {
				ICollider* a = &f;
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
					ICollider* a = &f;
					return a->CheckCollision(t, fixVec);
					}, f, t)) {
					return true;	// ヒットしていたのでループを終了
				}
			}
		}
	}

	return false;	// ヒットしていないのでfalseを返す
}

