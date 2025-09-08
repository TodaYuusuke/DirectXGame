#include "cRectangle.h"

#include "component/Resource.h"
#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Object::Collider2D;
using namespace LWP::Primitive;
using namespace LWP::Math;


// 移譲コンストラクタ
Rectangle::Rectangle() {
#if DEMO
	// 描画用のスプライトを作成
	sprite_.material.texture = Resource::LoadTextureLongPath("resources/system/texture/Rectangle.png");
#endif
}

void Rectangle::Update() {
#if DEMO
	Rectangle::Data d(*this);

	// 座標を合わせる
	sprite_.worldTF.translation.x = d.center.x;
	sprite_.worldTF.translation.y = d.center.y;
	// 回転を適応
	sprite_.worldTF.rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), rotation);
	// 128x128 テクスチャに対するスケール
	sprite_.worldTF.scale.x = size.x / 128.0f;
	sprite_.worldTF.scale.y = size.y / 128.0f;
	
	// 描画切り替え
	sprite_.isActive = isActive;
	sprite_.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif
}
Rectangle2D Rectangle::GetBoundingRect() {
	// そのまま返すだけ
	Rectangle::Data d(*this);
	Vector2 min = d.center - (d.size * 0.5f);
	Vector2 max = d.center + (d.size * 0.5f);
	return Rectangle2D{ min,max };
}

void Rectangle::DebugGUI() {
	ImGui::DragFloat3("center", &tf_->translation.x, 1.0f);
	ImGui::SliderFloat("rotation", &rotation, 0.0f, 6.28f);
	ImGui::DragFloat2("size", &size.x, 1.0f);
	ICollider::DebugGUI();
}
void Rectangle::Hit() {
#if DEMO
	sprite_.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Rectangle::Data::Data(const Rectangle& rect) {
	Vector3 worldPos = rect.tf_->GetWorldPosition();
	center = Vector2{ worldPos.x, worldPos.y };	// 中心座標
	size = rect.size;			// サイズ
	rotation = rect.rotation;	// 回転量
}

std::array<LWP::Math::Vector2, 4> Rectangle::Data::GetCorners() {
	using namespace LWP::Math;
	float c = cosf(rotation);
	float s = sinf(rotation);

	Vector2 axisX = { c, s };    // ローカルx軸
	Vector2 axisY = { -s, c };   // ローカルy軸
	Vector2 half = size * 0.5f;

	std::array<Vector2, 4> corners;
	corners[0] = center + axisX * half.x + axisY * half.y;
	corners[1] = center - axisX * half.x + axisY * half.y;
	corners[2] = center - axisX * half.x - axisY * half.y;
	corners[3] = center + axisX * half.x - axisY * half.y;
	return corners;
}