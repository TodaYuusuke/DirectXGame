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
	capsuleModel.Set();
	capsuleModel.material.enableLighting = false;
	capsuleModel.isWireFrame = true;
#endif
};
// コピーコンストラクタ
Capsule::Capsule(const Capsule& other) {
	*this = other;

#if DEMO
	// カプセルモデルを生成
	capsuleModel.Set();
	capsuleModel.material.enableLighting = false;
	capsuleModel.isWireFrame = true;
#endif
}


void Capsule::Update() {
#if DEMO
	Data d(*this);
	capsuleModel.CreateFromCapsuleCol(d.start, d.end, d.radius);
	// isActive切り替え
	capsuleModel.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	capsuleModel.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	//if (!isActive) { return; }
}

void Capsule::Create(const LWP::Math::Vector3& start_, const LWP::Math::Vector3& end_) { Create(start_, end_, 1.0f); }
void Capsule::Create(const LWP::Math::Vector3& start_, const LWP::Math::Vector3& end_, const float& rad_) {
	start = start_;
	end = end_;
	radius = rad_;
}

void Capsule::DebugGUI() {
	ImGui::DragFloat3("start", &start.x, 0.01f);
	ImGui::DragFloat3("end", &end.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
	ICollider::DebugGUI();
}

void Capsule::Hit() {
#if DEMO
	// hitしているときは色を変える
	capsuleModel.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Capsule::Data::Data(Capsule& cap) {
	Vector3 worldPos = cap.follow_->GetWorldPosition();
	start = cap.start + worldPos;
	end = cap.end + worldPos;
	radius = cap.radius;
}
