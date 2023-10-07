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
		/// オブジェクトのインスタンスを作成
		/// </summary>
		/// <typeparam name="TObject">オブジェクトの種類</typeparam>
		/// <returns>オブジェクトのインスタンス</returns>
		template <IsIObject TObject>
		TObject* CreateInstance() {
			return System::engine->objectManager_->CreateInstance<TObject>();
		}
	};
};