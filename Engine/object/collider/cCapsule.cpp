#include "cCapsule.h"
#include "base/ImGuiManager.h"
#if DEMO
#include "component/Primitive.h"
#endif

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
	// 立方体のインスタンスを作成
	capsuleModel = LWP::Primitive::CreateInstance<Primitive::Capsule>();
	capsuleModel->CreateFromCapsuleCol(*this);
#endif
};

void Capsule::Create(const LWP::Math::Vector3& start_, const LWP::Math::Vector3& end_) { Create(start_, end_, 1.0f); }
void Capsule::Create(const LWP::Math::Vector3& start_, const LWP::Math::Vector3& end_, const float& rad_) {
	start = start_;
	end = end_;
	radius = rad_;
}

#if DEMO
void Capsule::ShowWireFrame() {
	// isActive切り替え
	capsuleModel->isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	capsuleModel->commonColor = new Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
};
#endif

void Capsule::UpdateShape() {
#if DEMO
	capsuleModel->CreateFromCapsuleCol(*this);	// Capsule再生成
#endif
}

void Capsule::DerivedDebugGUI() {
	ImGui::DragFloat3("start", &start.x, 0.01f);
	ImGui::DragFloat3("end", &end.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
}
