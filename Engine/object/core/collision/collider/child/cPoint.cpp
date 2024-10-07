#include "cPoint.h"

#include "primitive/3d/Sphere.h"
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
	// モデルの初期設定
	sphere.subdivision = 4;
	sphere.radius = 0.02f;
	sphere.material.enableLighting = false;
	sphere.isWireFrame = true;
#endif
}

Point::Point(const Point& other) {
	*this = other;

#if DEMO
	// モデルの初期設定
	sphere.subdivision = 4;
	sphere.radius = 0.02f;
	sphere.material.enableLighting = false;
	sphere.isWireFrame = true;
#endif
}

void Point::Update() {
#if DEMO
	sphere.worldTF.translation = position + follow_->GetWorldPosition();
	// isActive切り替え
	sphere.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	sphere.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	//if (!isActive) { return; }
}
void Point::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// そのまま返すだけ
	*minPtr = *maxPtr = position + follow_->GetWorldPosition();
}

void Point::DebugGUI() {
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ICollider::DebugGUI();
}

void Point::Hit() {
#if DEMO
	// hitしているときは色を変える
	sphere.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Point::Data::Data(Point& point) {
	position = point.position + point.follow_->GetWorldPosition();
}