#include "cPoint.h"
#include "cAABB.h"
#include "cSphere.h"
#include "cCapsule.h"
#include "cMesh.h"

#include "utility/MyUtility.h"

using namespace LWP::Object::Collider;
using namespace LWP::Math;
using namespace LWP;

void CallHit(ICollisionShape* c1, ICollisionShape* c2, const bool& flag) {
#if DEMO
	if (flag) {
		c1->Hit();
		c2->Hit();
	}
#else
	c1; c2; flag;
#endif
}

// 定義の順番
// Point
// AABB
// Sphere
// Capsule
// Mesh


#pragma region Point * other
// ALL 未実装
bool Point::CheckCollision(Point& c) {
	Point::Data data1(*this);
	Point::Data data2(c);
	
	bool result = data1.position == data2.position;

	CallHit(this, &c, result);
	return result;
}
bool Point::CheckCollision(AABB& c) {
	Point::Data point(*this);
	AABB::Data aabb(c);

	bool result =
		(point.position.x >= aabb.min.x && point.position.x <= aabb.max.x) &&
		(point.position.y >= aabb.min.y && point.position.y <= aabb.max.y) &&
		(point.position.z >= aabb.min.z && point.position.z <= aabb.max.z);

	CallHit(this, &c, result);
	return result;
}
bool Point::CheckCollision(Sphere& c) {
	Point::Data point(*this);
	Sphere::Data s(c);

	bool result = (point.position - s.position).Length() <= s.radius;

	CallHit(this, &c, result);
	return result;
}
bool Point::CheckCollision(Capsule& c) {
	Utility::Log("Error!! Point * Capsule Collision is Unimplemented");
	c; return false;
}
bool Point::CheckCollision(Mesh& c) { return c.CheckCollision(*this); }

#pragma endregion

#pragma region AABB * other
// ALL OK
bool AABB::CheckCollision(Point& c) { return c.CheckCollision(*this); }
bool AABB::CheckCollision(AABB& c) {
	AABB::Data data1(*this);
	AABB::Data data2(c);
	bool result =
		(data1.min.x <= data2.max.x && data1.max.x >= data2.min.x) &&
		(data1.min.y <= data2.max.y && data1.max.y >= data2.min.y) &&
		(data1.min.z <= data2.max.z && data1.max.z >= data2.min.z);

	CallHit(this, &c, result);
	return result;
}
bool AABB::CheckCollision(Sphere& c) {
	AABB::Data aabb(*this);
	Sphere::Data sphere(c);

	// 最近接点
	Vector3 closestPoint = {
		std::clamp(sphere.position.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.position.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.position.z, aabb.min.z, aabb.max.z),
	};

	float dist = (closestPoint - sphere.position).Length();
	bool result = dist <= sphere.radius;

	CallHit(this, &c, result);
	return result;
}
bool AABB::CheckCollision(Capsule& c) {
	AABB::Data aabb(*this);
	Capsule::Data capsule(c);

	Vector3 d = aabb.center - capsule.start;
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
	f.x = (1.0f - t) * capsule.start.x + t * capsule.end.x;
	f.y = (1.0f - t) * capsule.start.y + t * capsule.end.y;
	f.z = (1.0f - t) * capsule.start.z + t * capsule.end.z;

	// 最近接点
	Vector3 closestPoint = {
		std::clamp(f.x, aabb.min.x, aabb.max.x),
		std::clamp(f.y, aabb.min.y, aabb.max.y),
		std::clamp(f.z, aabb.min.z, aabb.max.z),
	};

	float dist = (closestPoint - f).Length();
	bool result = dist <= capsule.radius;

	CallHit(this, &c, result);
	return result;
}
bool AABB::CheckCollision(Mesh& c) { return c.CheckCollision(*this); }

#pragma endregion

#pragma region Sphere * other
// ALL OK
bool Sphere::CheckCollision(Point& c) { return c.CheckCollision(*this); }
bool Sphere::CheckCollision(AABB& c) { return c.CheckCollision(*this); }
bool Sphere::CheckCollision(Sphere& c) {
	Sphere::Data data1(*this);	// transformをかけたデータで計算する
	Sphere::Data data2(c);

	// 二つの球体の中心点間の距離を求める
	Vector3 dist = data1.position - data2.position;
	// 半径の合計よりも短ければ衝突
	bool result = dist.Length() <= (data1.radius + data2.radius);

	CallHit(this, &c, result);
	return result;
}
bool Sphere::CheckCollision(Capsule& c) {
	Sphere::Data sphere(*this);
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
	bool result = distance < sphere.radius + capsule.radius;

	CallHit(this, &c, result);
	return result;
}
bool Sphere::CheckCollision(Mesh& c) { return c.CheckCollision(*this); }

#pragma endregion

#pragma region Capsule * other
// Capsule * Capsule 未実装
bool Capsule::CheckCollision(Point& c) { return c.CheckCollision(*this); }
bool Capsule::CheckCollision(AABB& c) { return c.CheckCollision(*this); }
bool Capsule::CheckCollision(Sphere& c) { return c.CheckCollision(*this); }
bool Capsule::CheckCollision(Capsule& c) { Utility::Log("Error!! Capsule * Capsule Collision is Unimplemented"); c; return false; }
bool Capsule::CheckCollision(Mesh& c) { return c.CheckCollision(*this); }

#pragma endregion

#pragma region Mesh * other

bool Mesh::CheckCollision(Point& c) { Utility::Log("Error!! Mesh * Point Collision is Unimplemented"); c; return false; }
bool Mesh::CheckCollision(AABB& c) { Utility::Log("Error!! Mesh * AABB Collision is Unimplemented"); c; return false; }
bool Mesh::CheckCollision(Sphere& c) { Utility::Log("Error!! Mesh * Sphere Collision is Unimplemented"); c; return false; }
bool Mesh::CheckCollision(Capsule& c) { Utility::Log("Error!! Mesh * Capsule Collision is Unimplemented"); c; return false; }
bool Mesh::CheckCollision(Mesh& c) { Utility::Log("Error!! Mesh * Mesh Collision is Unimplemented"); c; return false; }

#pragma endregion