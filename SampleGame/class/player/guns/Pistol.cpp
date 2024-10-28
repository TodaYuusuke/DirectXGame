#include "Pistol.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Info;

Pistol::Pistol() : capsule_(collision_.SetBroadShape(Collider::Capsule())) {}

void Pistol::Init(TransformQuat* wtf) {
	// モデル
	model_.LoadShortPath("Player/Guns/Pistol.gltf");
	model_.worldTF.Parent(wtf);
	model_.worldTF.translation = { 0.36f, -0.36f, 1.24f };
	// アニメーション
	anim_.LoadFullPath("resources/model/Player/Guns/Pistol.gltf", &model_);

	// コライダーの当たり判定設定
	collision_.mask.SetBelongFrag(MaskLayer::Bullet);
	collision_.mask.SetHitFrag(MaskLayer::Enemy);
	capsule_.radius = 0.2f;

	// レティクルのスプライト表示
	reticle_.material.texture = Resource::LoadTexture("Reticle.png");
	reticle_.anchorPoint = { 0.5f,0.5f };
	reticle_.worldTF.translation.x = 1920.0f / 2.0f;
	reticle_.worldTF.translation.y = 1080.0f / 2.0f;
}

void Pistol::Update() {
	// コライダーをオフにする
	collision_.isActive = false;
#if DEMO
	capsule_.isShowWireFrame = false;
#endif
}

void Pistol::Shot(LWP::Math::Vector3 pos, LWP::Math::Vector3 dir) {
	// アニメーション再生
	anim_.Play("01_Shot");

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
	if (ImGui::TreeNode("Animation")) {
		anim_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Collision")) {
		collision_.DebugGUI();
		ImGui::TreePop();
	}
}
