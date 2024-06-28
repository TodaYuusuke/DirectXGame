#include "cAABB.h"
#include "cSphere.h"
#include "cCapsule.h"

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"

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
#if DEMO
	capsuleModel.CreateFromCapsuleCol(*this);	// Capsule再生成
	// isActive切り替え
	capsuleModel.isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	capsuleModel.material.color = Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	//if (!isActive) { return; }
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


bool Capsule::CheckCollision(AABB* c) {
	return c->CheckCollision(this);
}
//bool CheckCollision(OBB* c)  override;
bool Capsule::CheckCollision(Sphere* c) {
	return c->CheckCollision(this);
}
bool Capsule::CheckCollision(Capsule* c) {
	Utility::Log("Error!! Capsule * Capsule Collision is Unimplemented");
	c; return false;
}
