#include "cCircle.h"
#include "cRectangle.h"

#include "base/ImGuiManager.h"

#include "utility/functions/MathFunctions.h"

using namespace LWP::Object::Collider2D;
using namespace LWP::Math;
using namespace LWP::Utility;
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
bool OverlapOnAxis(
	const std::array<Vector2, 4>& a,
	const std::array<Vector2, 4>& b,
	const Vector2& axis) {

	auto project = [&](const std::array<Vector2, 4>& corners) {
		float minP = std::numeric_limits<float>::infinity();
		float maxP = -std::numeric_limits<float>::infinity();
		for (auto& p : corners) {
			float proj = Vector2::Dot(p, axis);
			minP = std::min<float>(minP, proj);
			maxP = std::max<float>(maxP, proj);
		}
		return std::pair<float, float>{minP, maxP};
		};

	auto [minA, maxA] = project(a);
	auto [minB, maxB] = project(b);

	return !(maxA < minB || maxB < minA);
}


// 定義の順番
// Circle
// Rectangle


#pragma region Circle * other
bool Circle::CheckCollision(Circle& c, Math::Vector3* fixVec) {
	Circle::Data circle1(*this);
	Circle::Data circle2(c);
	
	bool result = Vector2::Distance(circle1.center, circle2.center) <= (circle1.radius + circle2.radius);

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
bool Circle::CheckCollision(Rectangle& c, Math::Vector3* fixVec) {
	Circle::Data circle(*this);
	Rectangle::Data rect(c);

	//　x軸とy軸の最短点を取得する
	Vector2 rectMin = rect.size * -0.5f;
	Vector2 rectMax = rect.size * 0.5f;
	// 矩形の回転を円にも適応させる
	Vector2 circleCenter = (circle.center - rect.center).Rotate(-rect.rotation);
	Vector2 clamp;	// 最近接点
	clamp.x = ClampOnRange(circleCenter.x, rectMin.x, rectMax.x);
	clamp.y = ClampOnRange(circleCenter.y, rectMin.y, rectMax.y);

	// 最近接点と円の中心の距離を求める
	bool result = Vector2::Distance(circleCenter, clamp) <= (circle.radius);

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
#pragma endregion

#pragma region Rectangle * other
bool Rectangle::CheckCollision(Circle& c, Math::Vector3* fixVec) { return c.CheckCollision(*this, fixVec); }
bool Rectangle::CheckCollision(Rectangle& c, Math::Vector3* fixVec) {
	Rectangle::Data rectA(*this);
	Rectangle::Data rectB(c);

	auto aCorners = rectA.GetCorners();
	auto bCorners = rectB.GetCorners();

	// 候補軸（各矩形の辺の方向）
	Vector2 axes[4];
	axes[0] = (aCorners[1] - aCorners[0]).Normalize();
	axes[1] = (aCorners[3] - aCorners[0]).Normalize();
	axes[2] = (bCorners[1] - bCorners[0]).Normalize();
	axes[3] = (bCorners[3] - bCorners[0]).Normalize();

	bool result = true;
	// 各軸に投影して重なりを確認
	for (int i = 0; i < 4; i++) {
		Vector2 axis = axes[i].Perp().Normalize(); // 法線方向
		if (!OverlapOnAxis(aCorners, bCorners, axis)) {
			result = false; // 分離軸あり → 非衝突
			break;
		}
	}

	CallHit(this, &c, result);
	*fixVec = { 0.0f,0.0f,0.0f };
	return result;
}
#pragma endregion

