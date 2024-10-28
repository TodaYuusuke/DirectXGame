#include "Pistol.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Info;

Pistol::Pistol() : capsule_(collision_.SetBroadShape(Collider::Capsule())) {}

void Pistol::Init(LWP::Object::TransformQuat* wtf) {
	model_.LoadShortPath("Player/Guns/Pistol/Pistol.gltf");
	model_.worldTF.Parent(wtf);
	model_.worldTF.translation = {0.36f, -0.36f, 1.24f};
	
	// コライダーの当たり判定設定
	collision_.mask.SetBelongFrag(MaskLayer::Bullet);
	collision_.mask.SetHitFrag(MaskLayer::Enemy);
	capsule_.radius = 0.2f;

	// モーション用意

}

void Pistol::Update() {
	// コライダーをオフにする
	collision_.isActive = false;
#if DEMO
	capsule_.isShowWireFrame = false;
#endif
}

void Pistol::Shot(LWP::Math::Vector3 pos, LWP::Math::Vector3 dir) {
	// 射撃地点から遠くまでカプセルコライダーを生成
	collision_.worldTF.translation = pos;
	capsule_.end = (dir * 100.0f);
	collision_.isActive = true;
#if DEMO
	capsule_.isShowWireFrame = true;
#endif
}
void Pistol::Reload() {
	// 装填数回復
	ammo_ = kMaxAmmo_;
}

void Pistol::DebugGUI() {
	if (ImGui::TreeNode("Model")) {
		model_.DebugGUI();
		ImGui::TreePop();
	}
	//if (ImGui::TreeNode("Animation")) {
	//	anim_.DebugGUI();
	//	ImGui::TreePop();
	//}
	if (ImGui::TreeNode("Collision")) {
		collision_.DebugGUI();
		ImGui::TreePop();
	}
}
