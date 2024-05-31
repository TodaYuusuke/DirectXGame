#include "cSphere.h"
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
	// 立方体のインスタンスを作成
	sphereModel.CreateFromSphereCol(*this);
#endif
}

void Sphere::Update() {
	// データが変わったら再生成
	//if (follow_.t && follow_.GetChanged()) {
	//	CreateFromPrimitive(follow_.t);
	//}
#if DEMO
	sphereModel.CreateFromSphereCol(*this);	// cube再生成
#endif
}

void Sphere::DebugGUI() {
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
}


void Sphere::Create(const LWP::Math::Vector3& pos) { Create(pos, 1.0f); }
void Sphere::Create(const LWP::Math::Vector3& pos, const float& rad) {
	position = pos;
	radius = rad;
}
// 形状から包み込む最小のAABBを生成する関数
void Sphere::CreateFromPrimitive(LWP::Primitive::IPrimitive* primitive) {
	// ワールドトランスフォームのペアレントもしておく
	//follow_ = primitive;
	//transform.Parent(&primitive->transform);
	// アフィン変換行列
	Matrix4x4 matrix = primitive->transform.GetAffineMatrix();
	// 初期化
	Vector3 min = primitive->vertices[0].position * matrix;
	Vector3 max = min;

	// 最小の値と最大の値を求める
	for (const Primitive::Vertex& vertex : primitive->vertices) {
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
