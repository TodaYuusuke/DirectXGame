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
		extern std::unique_ptr<Engine> engine;
		
		/// <summary>
		/// エンジン初期起動処理（※二重呼び出し禁止）
		/// </summary>
		void StartUp(std::wstring windowTitle);

		/// <summary>
		/// エンジン起動（※二重呼び出し禁止）
		/// </summary>
		void Run(IScene* firstScene);

		/// <summary>
		/// プログラム終了
		/// </summary>
		void End();

		
	};
};