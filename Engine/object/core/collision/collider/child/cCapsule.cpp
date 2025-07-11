#include "cCapsule.h"

#include "base/ImGuiManager.h"
#include "resources/model/SkinningModel.h"


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
	//capsuleModel.Set();
	//capsuleModel.material.enableLighting = false;
	//capsuleModel.isWireFrame = true;
#endif
};
// コピーコンストラクタ
Capsule::Capsule(const Capsule& other) {
	*this = other;

#if DEMO
	//// カプセルモデルを生成
	//capsuleModel.Set();
	//capsuleModel.material.enableLighting = false;
	//capsuleModel.isWireFrame = true;
#endif
}


void Capsule::Update() {
#if DEMO
	//Data d(*this);
	//capsuleModel.CreateFromCapsuleCol(d.start, d.end, d.radius);
	//// isActive切り替え
	//capsuleModel.isActive = isShowWireFrame && isActive;
	//// 色を白に戻す
	//capsuleModel.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	//if (!isActive) { return; }
}
void Capsule::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// 最大値店と最小地点を求める
	Vector3 worldPos = tf_->GetWorldPosition();
	Vector3 min = start + worldPos;
	Vector3 max = end + worldPos;
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
	//capsuleModel.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Capsule::Data::Data(Capsule& cap) {
	Matrix4x4 wtf = cap.tf_->GetAffineMatrix();/*
	switch (cap.follow_->type) {
		case FollowTargetType::None:
			wtf = cap.tf_->GetAffineMatrix();
			break;
		case FollowTargetType::Transform:
			wtf = cap.tf_->GetAffineMatrix();
			break;
		case FollowTargetType::Joint:
			wtf = cap.follow_->transform->GetAffineMatrix() * cap.follow_->model->worldTF.GetAffineMatrix() * cap.tf_->GetAffineMatrix();
			break;
	}*/
	start = cap.start * wtf;
	end = cap.end * wtf;
	radius = cap.radius;
}
