#include "cAABB.h"
#include "cSphere.h"
#include "cCapsule.h"

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
	sphereModel.material.enableLighting = false;
	sphereModel.isWireFrame = true;
#endif
}

void Sphere::Update() {
	ICollider::Update();
#if DEMO
	sphereModel.CreateFromSphereCol(*this);	// cube再生成
	// isActive切り替え
	sphereModel.isActive = isShowWireFrame && isActive;
	// hitしているときは色を変える
	sphereModel.material.color = Utility::Color(preHit ? Utility::ColorPattern::RED : Utility::ColorPattern::WHITE);
#endif

	// アクティブがOff -> 早期リターン
	if (!isActive) { return; }

	// データが変わったら再生成
	if (followModel_.t && followModel_.GetChanged()) {
		Create(followModel_.t);
	}
}

void Sphere::DebugGUI() {
	ICollider::DebugGUI();
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ImGui::DragFloat("radius", &radius, 0.01f);
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


bool Sphere::CheckCollision(AABB* c) {
	return c->CheckCollision(this);
}
//bool CheckCollision(OBB* c) {}
bool Sphere::CheckCollision(Sphere* c) {
	Sphere::Data data1(this);	// transformをかけたデータで計算する
	Sphere::Data data2(c);

	// 二つの球体の中心点間の距離を求める
	Vector3 dist = data1.position - data2.position;
	// 半径の合計よりも短ければ衝突
	return dist.Length() <= (data1.radius + data2.radius);
}
bool Sphere::CheckCollision(Capsule* c) {
	Sphere::Data sphere(this);
	Capsule::Data capsule(c);

	Vector3 d = sphere.position - capsule.start;
	Vector3 ba = capsule.end - capsule.start;
	// カプセルのベクトルの長さ
	float length = ba.Length();
	// 正規化
	Vector3 e = ba.Normalize();
	// 内積
	float dot = Vector3::Dot(d, e);

	float t = dot / length;
	t = std::clamp<float>(t, 0.0f, 1.0f);
	// 線形補間
	Vector3 f;
	f = (1.0f - t) * capsule.start + t * capsule.end;

	// 距離
	float distance = (sphere.position - f).Length();

	// 当たっているかを判定
	return distance < sphere.radius + capsule.radius;
}

