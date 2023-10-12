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
		/// デバッグ情報を表示する
		/// </summary>
		void ShowDebugGUI();
	};
	namespace Info = Information;
};