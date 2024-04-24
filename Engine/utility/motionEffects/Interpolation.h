#pragma once
#include "math/vector/Vector3.h"
#include <vector>

namespace LWP::Utility::Interpolation {
	enum class Type : int {
		Lerp,
		Slerp,
		InterpCount	// 個数カウント用
	};
	// 線形補間
	//float Lerp(const float& start, const float& end, const float& t);
	Math::Vector3 Lerp(const Math::Vector3& start, const Math::Vector3& end,const float& t);
	// 球面線形補間
	//float Slerp(const float& start, const float& end, const float& t);
	Math::Vector3 Slerp(const Math::Vector3& start, const Math::Vector3& end, const float& t);
	// Cat-mullRom曲線
	//Math::Vector3 CatmullRomSpline(const std::vector<Math::Vector3>& controlPoints, const float& t);
};

// 短縮形
namespace LWP::Utility::Interp {
	using namespace Interpolation;
}