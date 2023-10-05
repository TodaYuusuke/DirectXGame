#pragma once
#include "../LWP.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// システム関連（※ユーザ呼び出し禁止）
	/// </summary>
	namespace System {
		/// <summary>
		/// エンジンのインスタンス
		/// </summary>
		static std::unique_ptr<Engine> engine_;

		/// <summary>
		/// エンジン起動（※ユーザ呼び出し禁止）
		/// </summary>
		static void Run();
	};
};