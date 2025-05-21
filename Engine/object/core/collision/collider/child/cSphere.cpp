#include "cSphere.h"

#include "resources/model/RigidModel.h"
#include "base/ImGuiManager.h"

using namespace LWP::Object::Collider;
using namespace LWP;
using namespace LWP::Math;

Sphere::Sphere() : Sphere({ 0.0f,0.0f,0.0f }, 1.0f) {};
Sphere::Sphere(const LWP::Math::Vector3& pos) : Sphere(pos, 1.0f) {};
Sphere::Sphere(const float& rad) : Sphere({ 0.0f,0.0f,0.0f }, rad) {};
Sphere::Sphere(const LWP::Math::Vector3& pos, const float& rad) {
	position = pos;
	radius = rad;

#if DEMO
	// モデルの初期設定
	sphereModel.material.enableLighting = false;
	sphereModel.isWireFrame = true;
#endif
}

Sphere::Sphere(const Sphere& other) {
	*this = other;

#if DEMO
	// モデルの初期設定
	sphereModel.material.enableLighting = false;
	sphereModel.isWireFrame = true;
#endif
}


void Sphere::Update() {
#if DEMO
	sphereModel.CreateFromSphereCol(position + tf_->GetWorldPosition(), radius);	// Sphere再生成
	// isActive切り替え
	sphereModel.isActive = isShowWireFrame && isActive;
	// 色を白に戻す
	sphereModel.material.color = Utility::Color(Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	//if (!isActive) { return; }
}
void Sphere::GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) {
	// 中心から半径分移動した地点がminとmax
	Vector3 center = position + tf_->GetWorldPosition();
	*minPtr = center - Vector3{ radius, radius, radius };
	*maxPtr = center + Vector3{ radius, radius, radius };
}

void Sphere::Create(const LWP::Math::Vector3& pos) { Create(pos, 1.0f); }
void Sphere::Create(const LWP::Math::Vector3& pos, const float& rad) {
	position = pos;
	radius = rad;
}
void Sphere::Create(LWP::Resource::RigidModel* model) {
	// 必要なデータを集める
	Matrix4x4 matrix = model->worldTF.GetAffineMatrix();	// アフィン変換行列
	std::vector<Primitive::Vertex> vertices = model->GetModelData()->GetVertices();
	// 初期化
	Vector3 min = vertices[0].position * matrix;
	Vector3 max = min;

	// 最小の値と最大の値を求める
	for (const Primitive::Vertex& vertex : vertices) {
		Vector3&& v = vertex.position * matrix;
		min.x = min.x > v.x ? v.x : min.x;
		min.y = min.y > v.y ? v.y : min.y;
		min.z = min.z > v.z ? v.z : min.z;
		max.x = max.x < v.x ? v.x : max.x;
		max.y = max.y < v.y ? v.y : max.y;
		max.z = max.z < v.z ? v.z : max.z;
	}
	// ここから中心点を割り出す
	position = (max + min) / 2.0f;
	// 半径を割り出す
	radius = (max - position).Length();
}

void Sphere::DebugGUI() {
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
	ICollider::DebugGUI();
}

void Sphere::Hit() {
#if DEMO
	// hitしているときは色を変える
	sphereModel.material.color = Utility::Color(Utility::ColorPattern::RED);
#endif
}

Sphere::Data::Data(Sphere& sphere) {
	position = sphere.position + sphere.tf_->GetWorldPosition();
	radius = sphere.radius;
}