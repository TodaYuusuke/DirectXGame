#include "cCircle.h"


#include "component/Resource.h"
#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Object::Collider2D;
using namespace LWP::Primitive;
using namespace LWP::Math;


// 移譲コンストラクタ
Circle::Circle() : Circle(50.0f) {}

Circle::Circle(const float& radius_) {
	radius = radius_;

#if DEMO
	// 描画用のスプライトを作成
	//sprite_ = new NormalSprite();
	sprite_.material.texture = Resource::LoadTextureLongPath("resources/system/texture/Circle.png");
#endif
}
Circle::~Circle() {
#if DEMO
	//delete sprite_;
	//sprite_ = nullptr;
#endif
}

void Circle::Update() {
#if DEMO
	Circle::Data d(*this);
	// 円に合わせてスプライトを変形させる
	float w = (d.radius * 2.0f);

	// 座標を合わせる
	sprite_.worldTF.translation.x = d.center.x;
	sprite_.worldTF.translation.y = d.center.y;
	// 128x128 テクスチャに対するスケール
	sprite_.worldTF.scale.x = w / 128.0f;
	sprite_.worldTF.scale.y = w / 128.0f;
	// 描画切り替え
	sprite_.isActive = isActive;
	sprite_.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif
}
Rectangle2D Circle::GetBoundingRect() {
	// そのまま返すだけ
	Circle::Data d(*this);
	return Rectangle2D{
		d.center - Vector2{radius, radius},
		d.center + Vector2{radius, radius}
	};
}

void Circle::DebugGUI() {
	ImGui::DragFloat2("center", &tf_->translation.x, 1.0f);
	ImGui::DragFloat("radius", &radius, 0.1f);
	ICollider::DebugGUI();
}
void Circle::Hit() {
#if DEMO
	sprite_.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Circle::Data::Data(const Circle& circle) {
	Vector3 worldPos = circle.tf_->GetWorldPosition();
	center.x = worldPos.x;
	center.y = worldPos.y;
	radius = circle.radius;
}