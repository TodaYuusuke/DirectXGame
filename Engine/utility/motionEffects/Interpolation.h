#pragma once
#include "math/vector/Vector3.h"
#include "math/Quaternion.h"
#include <vector>

namespace LWP::Utility::Interpolation {
	enum class Type : int {
		Lerp,
		Slerp,
		Exponential,
		InterpCount	// 個数カウント用
	};
	// 線形補間
	float LerpF(const float& start, const float& end, const float& t);
	Math::Vector3 Lerp(const Math::Vector3& start, const Math::Vector3& end,const float& t);
	// 球面線形補間
	//float SlerpF(const float& start, const float& end, const float& t);
	Math::Vector3 Slerp(const Math::Vector3& start, const Math::Vector3& end, const float& t);
	Math::Quaternion SlerpQuaternion(const Math::Quaternion& start, const Math::Quaternion& end, const float& t);
	// 指数補間
	Math::Vector3 Exponential(const Math::Vector3& current, const Math::Vector3& target, float damping);
	// Cat-mullRom曲線
	//Math::Vector3 CatmullRomSpline(const std::vector<Math::Vector3>& controlPoints, const float& t);
};

// 短縮形
namespace LWP::Utility::Interp {
	using namespace Interpolation;
}