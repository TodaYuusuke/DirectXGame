#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// 入力関連
	/// </summary>
	namespace Input {
		/// <summary>
		/// キーボード
		/// </summary>
		//namespace Keyboard {
			/// <summary>
			/// keyID が None のとき true を返す
			/// </summary>
			static void GetNone(uint8_t keyID);
			/// <summary>
			/// keyID が Trigger のとき true を返す
			/// </summary>
			static void GetTrigger(uint8_t keyID);
			/// <summary>
			/// keyID が Press のとき true を返す
			/// </summary>
			static void GetPress(uint8_t keyID);
			/// <summary>
			/// keyID が Release のとき true を返す
			/// </summary>
			static void GetRelease(uint8_t keyID);
		//};

		/// <summary>
		/// コントローラー 
		/// </summary>
		//namespace Controller {

		//};
	};
}