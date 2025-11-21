#include "cPoint.h"

#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;


// 移譲コンストラクタ
Point::Point() : Point({ 0.0f,0.0f,0.0f }) {}

Point::Point(const LWP::Math::Vector3& pos) {
	position = pos;

#if DEMO
	dPri.ChangeFillMode();	// ワイヤーフレームに
	dPri.SetAllMaterialLighting(false);
#endif
}

void Point::Update() {
#if DEMO
	dPri.CreateFromSphereCol(tf_->GetWorldPosition(), 0.01f);	// Sphere再生成
	// isActive切り替え
	dPri.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	dPri.materials["Material0"].color = Utility::Color(Utility::ColorPattern::WHITE);
#endif
}
void Point::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// そのまま返すだけ
	*minPtr = *maxPtr = position + tf_->GetWorldPosition();
}

void Point::DebugGUI() {
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ICollider::DebugGUI();
}

void Point::Hit() {
#if DEMO
	// hitしているときは色を変える
	dPri.materials["Material0"].color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Point::Data::Data(Point& point) {
	position = point.position + point.tf_->GetWorldPosition();
}