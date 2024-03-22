#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// 形状関連
	/// </summary>
	namespace Primitive {
		/// <summary>
		/// <para>モーションクラスのインスタンスをエンジンにセットする関数</para>
		/// <para>※ ユーザー呼び出し禁止</para>
		/// </summary>
		void SetInstance(IPrimitive* ptr);
		/// <summary>
		/// <para>モーションクラスのインスタンスをエンジンから解放する関数</para>
		/// <para>※ ユーザー呼び出し禁止</para>
		/// </summary>
		void DeleteInstance(IPrimitive* ptr);
	};
};