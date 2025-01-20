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
		//extern std::unique_ptr<Engine> engine;

		/// <summary>
		/// エンジン起動（※二重呼び出し禁止）
		/// </summary>
		template<class IsIScene>
		void Run(std::wstring windowTitle) {
			// エンジン生成
			Engine* engine = new Engine();
			engine->StartUp(windowTitle);
			// エンジン起動
			engine->Run(new IsIScene());
			// エンジン削除
			delete engine;

			// シングルトン解放
			LWP::Utility::Singleton::DestroyAll();
		}
	};
};