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
		/// デルタタイムを取得する関数
		/// </summary>
		/// <returns></returns>
		double GetDeltaTime();
		float GetDeltaTimeF();
		/// <summary>
		/// 係数を掛けていないデルタタイムを取得する関数
		/// </summary>
		/// <returns></returns>
		double GetDefaultDeltaTime();
		float GetDefaultDeltaTimeF();

		/// <summary>
		/// デルタタイムに掛ける係数を設定する関数
		/// </summary>
		/// <param name="value">係数の値：default ... 1.0f</param>
		void SetDeltaTimeMultiply(float value);

		/// <summary>
		/// DebugGUIの可視性フラグ
		/// </summary>
		extern bool isDebugGUIVisible;
	};
	namespace Info = Information;
};