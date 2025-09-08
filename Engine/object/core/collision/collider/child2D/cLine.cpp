#include "cLine.h"

#include "component/Resource.h"
#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Object::Collider2D;
using namespace LWP::Primitive;
using namespace LWP::Math;


// 移譲コンストラクタ
Line::Line() : Line({ 0.0f,0.0f }, { 10.0f,10.0f }) {}

Line::Line(const LWP::Math::Vector2& start_, const LWP::Math::Vector2& end_) {
	start = start;
	end = end_;

#if DEMO
	// 描画用のスプライトを作成
	sprite_.material.texture = Resource::LoadTextureLongPath("resources/system/texture/Line.png");
#endif
}

void Line::Update() {
#if DEMO
	// 始点と終点に会わせてスプライトを変形させる


	//Vector3 worldPos = tf_->GetWorldPosition();
	//Vector3 worldMin = min + worldPos;
	//Vector3 worldMax = max + worldPos;
	//cube.CreateVertices(worldMin, worldMax);	// cube再生成
	////follow_->rotation.Init();
	//// isActive切り替え
	//cube.isActive = isShowWireFrame && isActive;
	//// 色を白に戻す
	//cube.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif
}
Rectangle2D Line::GetBoundingRect() {
	// そのまま返すだけ
	Line::Data d(*this);
	*minPtr = d.min;
	*maxPtr = d.max;
}

void AABB::Create(const LWP::Math::Vector3& position) { Create(position, { 1.0f,1.0f,1.0 }); }
void AABB::Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size) {
	// サイズの値を求める
	LWP::Math::Vector3 s = size / 2.0f;
	min = (s * -1.0f) + position;
	max = s + position;
}

void Line::DebugGUI() {
	ImGui::DragFloat2("start", &start.x, 0.01f);
	ImGui::DragFloat2("end", &end.x, 0.01f);
	ICollider::DebugGUI();
}

Line::Data::Data(const Line& line) {
	Matrix4x4 affine = line.tf_->GetAffineMatrix();
	Vector3 worldPos = line.tf_->GetWorldPosition();

	start = Vector2{ worldPos.x, worldPos.y } + line.start;	// アフィンの移動後にずらす
	end = ((line.end - line.start) * affine) + line.start;	// アフィン変換を優先
}