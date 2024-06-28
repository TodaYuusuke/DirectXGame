#include "cAABB.h"
//#include "cSphere.h"
//#include "cCapsule.h"

#include "resources/model/RigidModel.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;


// 移譲コンストラクタ
AABB::AABB() : AABB({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f }) {}

AABB::AABB(const LWP::Math::Vector3& min_, const LWP::Math::Vector3& max_) {
	min = min_;
	max = max_;

#if DEMO
	// 立方体のインスタンスを作成
	cube.CreateFromAABB(*this);
	cube.material.enableLighting = false;
	cube.isWireFrame = true;
#endif
}

void AABB::Update() {
#if DEMO
	cube.CreateFromAABB(*this);	// cube再生成
	cube.worldTF.Parent(follow_);
	follow_->rotation.Init();
	cube.worldTF.rotation.Init();
	// isActive切り替え
	cube.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	cube.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	if (!isActive) { return; }


	// MinよりMaxのほうが小さくならないように修正
	min.x = std::min<float>(min.x, max.x);
	min.y = std::min<float>(min.y, max.y);
	min.z = std::min<float>(min.z, max.z);

	max.x = std::max<float>(min.x, max.x);
	max.y = std::max<float>(min.y, max.y);
	max.z = std::max<float>(min.z, max.z);
}

void AABB::DebugGUI() {
	ICollisionShape::DebugGUI();
	ImGui::DragFloat3("min", &min.x, 0.01f);
	ImGui::DragFloat3("max", &max.x, 0.01f);
}

void AABB::Create(const LWP::Math::Vector3& position) { Create(position, { 1.0f,1.0f,1.0 }); }
void AABB::Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size) {
	// サイズの値を求める
	LWP::Math::Vector3 s = size / 2.0f;
	min = (s * -1.0f) + position; 
	max = s + position;
}

void AABB::Create(const LWP::Resource::RigidModel& model) { 
	// 必要なデータを集める
	Matrix4x4 matrix = model.worldTF.GetAffineMatrix();	// アフィン変換行列
	std::vector<Vertex> vertices = model.GetModelData()->GetVertices();
	// 初期化
	min = vertices[0].position * matrix;
	max = min;

	// 最小の値と最大の値を求める
	for (const Vertex& vertex : vertices) {
		Vector3&& v = vertex.position * matrix;
		min.x = min.x > v.x ? v.x : min.x;
		min.y = min.y > v.y ? v.y : min.y;
		min.z = min.z > v.z ? v.z : min.z;
		max.x = max.x < v.x ? v.x : max.x;
		max.y = max.y < v.y ? v.y : max.y;
		max.z = max.z < v.z ? v.z : max.z;
	}
}


bool AABB::CheckCollision(AABB& c) {
	AABB::Data data1(*this);
	AABB::Data data2(c);
	bool result =
		(data1.min.x <= data2.max.x && data1.max.x >= data2.min.x) &&
		(data1.min.y <= data2.max.y && data1.max.y >= data2.min.y) &&
		(data1.min.z <= data2.max.z && data1.max.z >= data2.min.z);

#if DEMO
	if (result) {
		Hit();
		c.Hit();
	}
#endif
	return result;
}
//bool AABB::CheckCollision(Sphere& c) {
//	AABB::Data aabb(this);
//	Sphere::Data sphere(c);
//
//	// 最近接点
//	Vector3 closestPoint = {
//		std::clamp(sphere.position.x, aabb.min.x, aabb.max.x),
//		std::clamp(sphere.position.y, aabb.min.y, aabb.max.y),
//		std::clamp(sphere.position.z, aabb.min.z, aabb.max.z),
//	};
//
//	float dist = (closestPoint - sphere.position).Length();
//	return dist <= sphere.radius;
//}
//bool AABB::CheckCollision(Capsule& c) {
//	AABB::Data aabb(this);
//	Capsule::Data capsule(c);
//
//	Vector3 d = aabb.center - capsule.start;
//	Vector3 ba = capsule.end - capsule.start;
//	// カプセルのベクトルの長さ
//	float length = ba.Length();
//	// 正規化
//	Vector3 e = ba.Normalize();
//	// 内積
//	float dot = Vector3::Dot(d, e);
//
//	float t = dot / length;
//	t = std::clamp<float>(t, 0.0f, 1.0f);
//	// 線形補間
//	Vector3 f;
//	f.x = (1.0f - t) * capsule.start.x + t * capsule.end.x;
//	f.y = (1.0f - t) * capsule.start.y + t * capsule.end.y;
//	f.z = (1.0f - t) * capsule.start.z + t * capsule.end.z;
//
//	// 最近接点
//	Vector3 closestPoint = {
//		std::clamp(f.x, aabb.min.x, aabb.max.x),
//		std::clamp(f.y, aabb.min.y, aabb.max.y),
//		std::clamp(f.z, aabb.min.z, aabb.max.z),
//	};
//
//	float dist = (closestPoint - f).Length();
//	return dist <= capsule.radius;
//}

void AABB::Hit() {
#if DEMO
	// hitしているときは色を変える
	cube.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

AABB::Data::Data(AABB& aabb) {
	Matrix4x4 affine = aabb.follow_->GetScaleMatrix() * aabb.follow_->GetTranslationMatrix();
	min = aabb.min * affine;
	max = aabb.max * affine;
	center = (min + max) / 2.0f;
}