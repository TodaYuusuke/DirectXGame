#pragma once
#include "System.h"


/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// 汎用関連
	/// </summary>
	namespace Common {
		/// <summary>
		/// さまざまなクラスのインスタンスを作成
		/// </summary>
		/// <returns>クラスのインスタンス</returns>
		template<IsIPrimitive TPri>
		inline TPri* CreateInstance() {
			return System::engine->primitiveManager_->CreateInstance<TPri>(System::engine->directXCommon_->GetCommandManager());
		}
		template<IsIObject TOb>
		inline TOb* CreateInstance() {
			return System::engine->objectManager_->CreateInstance<TOb>();
		}
		template<IsICollider TCol>
		inline TCol* CreateInstance() {
			return System::engine->colliderManager_->CreateInstance<TCol>();
		}
	};
};