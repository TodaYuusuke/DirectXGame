#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// エンジンのデータ関連
	/// </summary>
	namespace Information {
		/// <summary>
		/// 現在の解像度を返す
		/// </summary>
		static int GetWindowWidth();
		static int GetWindowHeight();
		static float GetWindowWidthF();
		static float GetWindowHeightF();
		static Math::Vector2 GetWindow();
	};
	namespace Info = Information;
};