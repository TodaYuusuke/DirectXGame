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
		int GetWindowWidth();
		int GetWindowHeight();
		float GetWindowWidthF();
		float GetWindowHeightF();
		Math::Vector2 GetWindow();

		/// <summary>
		/// デバッグ情報の表示/非表示を切り替える
		/// </summary>
		void ChangeShowDebugGUI();
	};
	namespace Info = Information;
};