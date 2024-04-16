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
		/// <para>オブジェクトのインスタンスをエンジンにセットする関数</para>
		/// <para>※ ユーザー呼び出し禁止</para>
		/// </summary>
		void SetPointer(IObject* ptr);
		/// <summary>
		/// <para>オブジェクトのインスタンスをエンジンから解放する関数</para>
		/// <para>※ ユーザー呼び出し禁止</para>
		/// </summary>
		void DeletePointer(IObject* ptr);

		/// <summary>
		/// 当たり判定関連
		/// </summary>
		namespace Collider {
			/// <summary>
			/// <para>コライダーのインスタンスをエンジンにセットする関数</para>
			/// <para>※ ユーザー呼び出し禁止</para>
			/// </summary>
			void SetPointer(ICollider* ptr);
			/// <summary>
			/// <para>コライダーのインスタンスをエンジンから解放する関数</para>
			/// <para>※ ユーザー呼び出し禁止</para>
			/// </summary>
			void DeletePointer(ICollider* ptr);
		};
	};
};