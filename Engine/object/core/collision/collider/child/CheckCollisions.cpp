#include "cPoint.h"
#include "cAABB.h"
#include "cSphere.h"
#include "cCapsule.h"

#include "base/ImGuiManager.h"
#include <algorithm>

using namespace LWP::Object::Collider;
using namespace LWP::Math;
using namespace LWP;

void CallHit(ICollider* c1, ICollider* c2, const bool& flag) {
#if DEMO
	if (flag) {
		c1->Hit();
		c2->Hit();
	}
#else
	c1; c2; flag;
#endif
}
bool NotImplementedFunc(ICollider* c1, ICollider* c2, Vector3* v) {
	// ログ出力でVisualStudioがかなり重くなるので一時的にコメントアウト
	//Utility::Log(
	//	"Error!! " +
	//	kShapeString[static_cast<int>(c1->GetShape())] +
	//	" * " +
	//	kShapeString[static_cast<int>(c2->GetShape())] +
	//	" Capsule Collision is Unimplemented\n"
	//);
	c1, c2;
	*v = { 0.0f,0.0f,0.0f };

	return false;
}

// 定義の順番
// Point
// AABB
// Sphere
// Capsule
// Mesh
// Terrain


#pragma region Point * other
// Capsule 未実装
bool Point::CheckCollision(Point& c, Math::Vector3* fixVec) {
	Point::Data data1(*this);
	Point::Data data2(c);
	
	bool result = data1.position == data2.position;

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool Point::CheckCollision(AABB& c, Math::Vector3* fixVec) {
	Point::Data point(*this);
	AABB::Data aabb(c);

	bool result =
		(point.position.x >= aabb.min.x && point.position.x <= aabb.max.x) &&
		(point.position.y >= aabb.min.y && point.position.y <= aabb.max.y) &&
		(point.position.z >= aabb.min.z && point.position.z <= aabb.max.z);

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool Point::CheckCollision(Sphere& c, Math::Vector3* fixVec) {
	Point::Data point(*this);
	Sphere::Data s(c);

	bool result = (point.position - s.position).Length() <= s.radius;

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool Point::CheckCollision(Capsule& c, Math::Vector3* fixVec) {
	Point::Data point(*this);
	Capsule::Data capsule(c);

	Vector3 ba = capsule.end - capsule.start;
	Vector3 pa = point.position - capsule.start;
	float h = std::clamp(Vector3::Dot(pa, ba) / Vector3::Dot(ba, ba), 0.0f, 1.0f);
	float distance = (pa - ba * h).Length();

	bool result = distance <= capsule.radius;

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}

#pragma endregion

#pragma region AABB * other
// ALL OK
bool AABB::CheckCollision(Point& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool AABB::CheckCollision(AABB& c, Math::Vector3* fixVec) {
	AABB::Data data1(*this);
	AABB::Data data2(c);
	bool result =
		(data1.min.x <= data2.max.x && data1.max.x >= data2.min.x) &&
		(data1.min.y <= data2.max.y && data1.max.y >= data2.min.y) &&
		(data1.min.z <= data2.max.z && data1.max.z >= data2.min.z);

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool AABB::CheckCollision(Sphere& c, Math::Vector3* fixVec) {
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
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool AABB::CheckCollision(Capsule& c, Math::Vector3* fixVec) {
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
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}

#pragma endregion

#pragma region Sphere * other
// ALL OK
bool Sphere::CheckCollision(Point& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Sphere::CheckCollision(AABB& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Sphere::CheckCollision(Sphere& c, Math::Vector3* fixVec) {
	Sphere::Data data1(*this);	// transformをかけたデータで計算する
	Sphere::Data data2(c);

	// 二つの球体の中心点間の距離を求める
	Vector3 dist = data1.position - data2.position;
	// 半径の合計よりも短ければ衝突
	bool result = dist.Length() <= (data1.radius + data2.radius);

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool Sphere::CheckCollision(Capsule& c, Math::Vector3* fixVec) {
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
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}

#pragma endregion

#pragma region Capsule * other
// Capsule * Capsule 未実装
bool Capsule::CheckCollision(Point& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(AABB& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(Sphere& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(Capsule& c, Math::Vector3* fixVec) {
	Capsule::Data d1(*this);
	Capsule::Data d2(c);

	Vector3 d1_vec = d1.end - d1.start;
	Vector3 d2_vec = d2.end - d2.start;
	Vector3 r = d1.start - d2.start;

	float a = Vector3::Dot(d1_vec, d1_vec);
	float e = Vector3::Dot(d2_vec, d2_vec);
	float f = Vector3::Dot(d2_vec, r);

	float s = 0.0f, t = 0.0f;

	if (a <= 1e-6f && e <= 1e-6f) {
		s = t = 0.0f;
	}
	else if (a <= 1e-6f) {
		s = 0.0f;
		t = std::clamp(f / e, 0.0f, 1.0f);
	}
	else {
		float c_val = Vector3::Dot(d1_vec, r);
		if (e <= 1e-6f) {
			t = 0.0f;
			s = std::clamp(-c_val / a, 0.0f, 1.0f);
		}
		else {
			float b = Vector3::Dot(d1_vec, d2_vec);
			float denom = a * e - b * b;

			if (denom != 0.0f) {
				s = std::clamp((b * f - c_val * e) / denom, 0.0f, 1.0f);
			}
			else {
				s = 0.0f;
			}

			t = (b * s + f) / e;

			if (t < 0.0f) {
				t = 0.0f;
				s = std::clamp(-c_val / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f) {
				t = 1.0f;
				s = std::clamp((b - c_val) / a, 0.0f, 1.0f);
			}
		}
	}

	Vector3 p1 = d1.start + d1_vec * s;
	Vector3 p2 = d2.start + d2_vec * t;

	float distance = (p1 - p2).Length();
	bool result = distance <= (d1.radius + d2.radius);

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}

#pragma endregion
