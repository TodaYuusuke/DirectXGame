#include "cCapsule.h"

#include "base/ImGuiManager.h"
#include "resources/model/SkinningModel.h"


using namespace LWP::Object::Collider;
using namespace LWP;
using namespace LWP::Math;

Capsule::Capsule() : Capsule({ 0.0f,0.0f,1.0f }) {};
Capsule::Capsule(const Vector3& offset) : Capsule(offset, 0.5f) {};
Capsule::Capsule(const float& rad) : Capsule({ 0.0f,0.0f,1.0f }, rad) {};
Capsule::Capsule(const Vector3& offset, const float& rad) {
	localOffset = offset;
	radius = rad;

#if DEMO
	dPri.ChangeFillMode();	// ワイヤーフレームに
	dPri.SetAllMaterialLighting(false);
#endif
};


void Capsule::Update() {
#if DEMO
	dPri.CreateFromCapsuleCol(tf_->GetWorldPosition(), localOffset, radius);	// Sphere再生成
	// isActive切り替え
	dPri.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	dPri.SetAllMaterialColor(Utility::Color(Utility::ColorPattern::WHITE));
#endif
}
void Capsule::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// 最大値店と最小地点を求める
	Vector3 worldPos = tf_->GetWorldPosition();
	Vector3 min = worldPos;
	Vector3 max = worldPos + localOffset;
	// MinよりMaxのほうが小さくならないように修正
	min.x = std::min<float>(min.x, max.x);
	min.y = std::min<float>(min.y, max.y);
	min.z = std::min<float>(min.z, max.z);
	// MaxよりMinのほうが大きくならないように修正
	max.x = std::max<float>(min.x, max.x);
	max.y = std::max<float>(min.y, max.y);
	max.z = std::max<float>(min.z, max.z);

	// そのまま返す
	*minPtr = min - Vector3{ radius, radius, radius };
	*maxPtr = max + Vector3{ radius, radius, radius };
}

void Capsule::DebugGUI() {
	ImGui::DragFloat3("position", &tf_->translation.x, 0.01f);
	ImGui::DragFloat3("localEndOffset", &localOffset.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
	ICollider::DebugGUI();
}

void Capsule::Hit() {
#if DEMO
	// hitしているときは色を変える
	dPri.SetAllMaterialColor(Utility::Color(Utility::ColorPattern::RED));
#endif
}

Capsule::Data::Data(Capsule& cap) {
	Vector3 worldPos = cap.tf_->GetWorldPosition();
	//Matrix4x4 wtf = cap.tf_->GetAffineMatrix();
	start = cap.tf_->GetWorldPosition();
	end = start + cap.localOffset;
	radius = cap.radius;
}
