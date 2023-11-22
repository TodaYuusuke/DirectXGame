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
		namespace Keyboard {
			/// <summary>
			/// keyID が None のとき true を返す
			/// </summary>
			bool GetNone(uint8_t keyID);
			/// <summary>
			/// keyID が Trigger のとき true を返す
			/// </summary>
			bool GetTrigger(uint8_t keyID);
			/// <summary>
			/// keyID が Press のとき true を返す
			/// </summary>
			bool GetPress(uint8_t keyID);
			/// <summary>
			/// keyID が Release のとき true を返す
			/// </summary>
			bool GetRelease(uint8_t keyID);
		};

		/// <summary>
		/// コントローラー 
		/// </summary>
		namespace Controller {
			/// <summary>
			/// playerNum のコントローラーの keyID が None のとき true を返す
			/// </summary>
			bool GetNone(int playerNum, int keyID);
			/// <summary>
			/// playerNum のコントローラーの keyID が Trigger のとき true を返す
			/// </summary>
			bool GetTrigger(int playerNum, int keyID);
			/// <summary>
			/// playerNum のコントローラーの keyID が Press のとき true を返す
			/// </summary>
			bool GetPress(int playerNum, int keyID);
			/// <summary>
			/// playerNum のコントローラーの keyID が Release のとき true を返す
			/// </summary>
			bool GetRelease(int playerNum, int keyID);

			/// <summary>
			/// playerNum のコントローラーのLTの押し込みの強さを返す
			/// <para>0.0f ~ 1.0f</para>
			/// </summary>
			float GetLT(int playerNum);
			/// <summary>
			/// playerNum のコントローラーのRTの押し込みの強さを返す
			/// <para>0.0f ~ 1.0f</para>
			/// </summary>
			float GetRT(int playerNum);

			/// <summary>
			/// Lスティックの入力を受け取る関数
			/// <para>| 　+Y　 |</para>
			/// <para>| -X　+X |</para>
			/// <para>| 　-Y　 |</para>
			/// </summary>
			Math::Vector2 GetLStick(int playerNum);
			/// <summary>
			/// Rスティックの入力を受け取る関数
			/// <para>| 　+Y　 |</para>
			/// <para>| -X　+X |</para>
			/// <para>| 　-Y　 |</para>
			/// </summary>
			Math::Vector2 GetRStick(int playerNum);

			/// <summary>
			/// デッドゾーンを設定
			/// </summary>
			void SetDeadZone(int playerNum, float value);
			/// <summary>
			/// 振動をセットする
			/// </summary>
			/// <param name="bigVibrationPower">大きい振動を起こすモータの速度 ... 0.0f ~ 1.0f</param>
			/// <param name="smallVibrationPower">細かな振動を起こすモータの速度 ... 0.0f ~ 1.0f</param>
			void SetVibration(int playerNum, float bigVibrationPower, float smallVibrationPower);
		};
		namespace Pad = Controller;
	};
}