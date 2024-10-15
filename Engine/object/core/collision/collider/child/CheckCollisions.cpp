#include "cPoint.h"
#include "cAABB.h"
#include "cSphere.h"
#include "cCapsule.h"
#include "cMesh.h"
#include "cTerrain.h"

#include "base/ImGuiManager.h"

#include "utility/MyUtility.h"

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
bool Point::CheckCollision(Capsule& c, Math::Vector3* fixVec) { Utility::Log("Error!! Point * Capsule Collision is Unimplemented"); c; fixVec; return false; }
bool Point::CheckCollision(Mesh& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Point::CheckCollision(Terrain& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }

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
bool AABB::CheckCollision(Mesh& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool AABB::CheckCollision(Terrain& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }

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
bool Sphere::CheckCollision(Mesh& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Sphere::CheckCollision(Terrain& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }

#pragma endregion

#pragma region Capsule * other
// Capsule * Capsule 未実装
bool Capsule::CheckCollision(Point& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(AABB& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(Sphere& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(Capsule& c, Math::Vector3* fixVec) { Utility::Log("Error!! Capsule * Capsule Collision is Unimplemented"); c; fixVec; return false; }
bool Capsule::CheckCollision(Mesh& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Capsule::CheckCollision(Terrain& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }

#pragma endregion

#pragma region Mesh * other
// ほぼ未実装（点のみ実装済み）
bool Mesh::CheckCollision(Point& c, Math::Vector3* fixVec) {
	Point::Data point(c);
	// 結果
	struct Result {
		bool b = false;
		int index = -1;
		
	}result;
	// 移動方向のベクトル
	Vector3 fixNormal = { 0.0f,0.0f,0.0f };
	// 面と点の距離
	float fixDist = 999999999.0f;

	// ※凹み形状には未対応

	// 全三角形検証
	for (const TriangleData& d : data) {
		// 三角形の法線と中心点の内積
		float dot = Vector3::Dot(d.normal ,d.center);
		// 平面に対する符号付距離
		float dist = (Vector3::Dot(d.normal, point.position) - dot) / Vector3::Dot(d.normal, d.normal);

		// 距離が0以上の場合は内部にないとして早期終了
		if (dist < 0.0f) {
			return false;
		}

		// 距離がより小さい場合
		if (dist < fixDist) {
			// 修正ベクトル計算用に保持
			fixNormal = d.normal;
			fixDist = dist;
		}
	}

	CallHit(this, &c, true);
	*fixVec = fixNormal * (-fixDist);
	return true;
}
bool Mesh::CheckCollision(AABB& c, Math::Vector3* fixVec) { Utility::Log("Error!! Mesh * AABB Collision is Unimplemented"); c; fixVec; return false; }
bool Mesh::CheckCollision(Sphere& c, Math::Vector3* fixVec) { Utility::Log("Error!! Mesh * Sphere Collision is Unimplemented"); c; fixVec; return false; }
bool Mesh::CheckCollision(Capsule& c, Math::Vector3* fixVec) { Utility::Log("Error!! Mesh * Capsule Collision is Unimplemented"); c; fixVec; return false; }
bool Mesh::CheckCollision(Mesh& c, Math::Vector3* fixVec) { Utility::Log("Error!! Mesh * Mesh Collision is Unimplemented"); c; fixVec; return false; }
bool Mesh::CheckCollision(Terrain& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }

#pragma endregion

#pragma region Terrain * other
// ALL 未実装
bool Terrain::CheckCollision(Point& c, Math::Vector3* fixVec) {
	Point::Data pos(c);

	// 検索するモートン番号
	const uint32_t kTargetMorton = quadtree_.GetMortonNumber(pos.position);
	if (kTargetMorton == -1) { return false; }	// -1だった場合早期終了
	uint32_t currentMorton = kTargetMorton;	// 現在のモートン番号（下位レベルから検証）
	uint32_t currentSpaceLevel = quadtree_.divisionLevel;	// 現在の空間レベル（下位レベルから検証）
	
	while (true) {
		// 現在の空間内の全オブジェクトと検証
		for (const Polygon& p : polygonMap_[currentMorton]) {
			// 平面のパラメータ
			float distance = p.normal.x * p.pos[0].x + p.normal.y * p.pos[0].y + p.normal.z * p.pos[0].z;
			// 垂直の場合はヒットしていない
			if (Vector3::Dot(p.normal, Vector3::UnitY()) == 0.0f) { continue; }

			// 媒介変数tを求める
			float t = (distance - Vector3::Dot(pos.position, p.normal)) / Vector3::Dot(Vector3::UnitY(), p.normal);
			// 衝突点を求める
			Vector3 hitPosition = pos.position + (t * Vector3::UnitY());

			// 各辺を結んだベクトルと頂点と衝突点pを結んだベクトルのクロス積を取る
			Vector3 cross01 = Vector3::Cross((p.pos[0] - p.pos[1]), (p.pos[1] - hitPosition));
			Vector3 cross12 = Vector3::Cross((p.pos[1] - p.pos[2]), (p.pos[2] - hitPosition));
			Vector3 cross20 = Vector3::Cross((p.pos[2] - p.pos[0]), (p.pos[0] - hitPosition));

			// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突可能
			if (Vector3::Dot(cross01, p.normal) >= 0.0f && Vector3::Dot(cross12, p.normal) >= 0.0f && Vector3::Dot(cross20, p.normal) >= 0.0f) {
				// 衝突点がposより上だった場合 -> 座標を修正
				if (hitPosition.y > pos.position.y) {
					*fixVec = hitPosition - pos.position;
					// 命中したので戻る
					CallHit(this, &c, true);
					return true;
				}
			}
		}
		// 検証したモートン番号が0だった場合終了
		if (currentMorton == 0) {
			break;
		}
		// 検証が終わったので上の空間レベルへ
		currentMorton = (currentMorton - quadtree_.GetSpaceLevelObjectsSum(currentSpaceLevel--)) >> 2;
		currentMorton += quadtree_.GetSpaceLevelObjectsSum(currentSpaceLevel);
	};
	
	// 命中しなかったのでfalse
	return false;
}
bool Terrain::CheckCollision(AABB& c, Math::Vector3* fixVec) { Utility::Log("Error!! Terrain * AABB Collision is Unimplemented"); c; fixVec; return false; }
bool Terrain::CheckCollision(Sphere& c, Math::Vector3* fixVec) { Utility::Log("Error!! Terrain * Sphere Collision is Unimplemented"); c; fixVec; return false; }
bool Terrain::CheckCollision(Capsule& c, Math::Vector3* fixVec) { Utility::Log("Error!! Terrain * Capsule Collision is Unimplemented"); c; fixVec; return false; }
bool Terrain::CheckCollision(Mesh& c, Math::Vector3* fixVec) { Utility::Log("Error!! Terrain * Mesh Collision is Unimplemented"); c; fixVec; return false; }
bool Terrain::CheckCollision(Terrain& c, Math::Vector3* fixVec) { Utility::Log("Error!! Terrain * Terrain Collision is Unimplemented"); c; fixVec; return false; }

#pragma endregion