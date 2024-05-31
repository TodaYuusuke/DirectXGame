#include "cCapsule.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP;
using namespace LWP::Math;

Capsule::Capsule() : Capsule({ 0.0f,0.0f ,0.0f }, { 0.0f,1.0f,0.0f }, 1.0f) {};
Capsule::Capsule(const LWP::Math::Vector3& start) : Capsule(start, { 0.0f,1.0f,0.0f }, 1.0f) {};
Capsule::Capsule(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end) : Capsule(start, end, 1.0f) {};
Capsule::Capsule(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, const float& rad_) {
	this->start = start;
	this->end = end;
	this->radius = rad_;

#if DEMO
	// カプセルモデルを生成
	capsuleModel.CreateFromCapsuleCol(*this);
	capsuleModel.material.enableLighting = false;
	capsuleModel.isWireFrame = true;
#endif
};


void Capsule::Update() {
	ICollider::Update();
	// アクティブがOff -> 早期リターン
	if (!isActive) { return; }

#if DEMO
	capsuleModel.CreateFromCapsuleCol(*this);	// Capsule再生成
	// isActive切り替え
	capsuleModel.isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	capsuleModel.material.color = Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
#endif

}

void Capsule::DebugGUI() {
	ICollider::DebugGUI();

	ImGui::DragFloat3("start", &start.x, 0.01f);
	ImGui::DragFloat3("end", &end.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
}



void Capsule::Create(const LWP::Math::Vector3& start_, const LWP::Math::Vector3& end_) { Create(start_, end_, 1.0f); }
void Capsule::Create(const LWP::Math::Vector3& start_, const LWP::Math::Vector3& end_, const float& rad_) {
	start = start_;
	end = end_;
	radius = rad_;
}
