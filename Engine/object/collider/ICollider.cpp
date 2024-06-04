#include "ICollider.h"
#include "base/ImGuiManager.h"

#include "component/Object.h"

using namespace LWP::Object::Collider;

// コンストラクタ
ICollider::ICollider() {
	SetPointer(this);
}
// デストラクタ
ICollider::~ICollider() {
	DeletePointer(this);
}

void ICollider::Update() {
	// preHit更新
	preHit = hit;
	hit = false;	// この後にヒット判定をするのでとりまfalse
}

// ヒット時に関数を呼び出す関数（※ユーザー呼び出し禁止）
void ICollider::ExecuteLambda(ICollider* hitCollision) {
	switch (static_cast<OnCollisionState>((preHit << 1) + hit))
	{
		case LWP::Object::Collider::OnCollisionState::NoHit:
			noHitLambda({ this, hitCollision });
			break;
		case LWP::Object::Collider::OnCollisionState::Enter:
			enterLambda({ this, hitCollision });
			break;
		case LWP::Object::Collider::OnCollisionState::Stay:
			stayLambda({ this, hitCollision });
			break;
		case LWP::Object::Collider::OnCollisionState::Exit:
			exitLambda({ this, hitCollision });
			break;
		default:
			break;
	}
}

void ICollider::DebugGUI() {
	// 派生クラス用
	//DerivedDebugGUI();
	//ImGui::Text("- Below this are common variables - ");
	// 追従先のワールドトランスフォーム
	if (followModel_.t && ImGui::TreeNode("FollowTarget Info")) {
		followModel_.t->DebugGUI();
		ImGui::TreePop();
	}
	
	ImGui::Checkbox("isMove", &isMove);	// 動くかのフラグ
	ImGui::Checkbox("isActive", &isActive);	// 有効/無効
#if DEMO
	ImGui::Checkbox("isShowWireFrame", &isShowWireFrame);	// デバッグ用にWireFrameを表示するか
#endif
}