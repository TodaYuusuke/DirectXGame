#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// ゲームオブジェクト関連
	/// </summary>
	namespace Object {

		/// <summary>
		/// 平行光源のポインタを返す関数
		/// </summary>
		inline DirectionLight* GetDirLight() { return Object::Manager::GetInstance()->GetDirLight(); }

	};
};