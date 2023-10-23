#pragma once
#include "../Engine/math/vector/Vector2.h"

#pragma region DirectInput

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#pragma endregion

#include <wrl.h>
#include <stdint.h>

#pragma region KeyID

// 各ボタンID
#define DIXBOX_A		DIJOFS_BUTTON(0);	// Aボタン
#define DIXBOX_B		DIJOFS_BUTTON(1);	// Bボタン
#define DIXBOX_X		DIJOFS_BUTTON(2);	// Xボタン
#define DIXBOX_Y		DIJOFS_BUTTON(3);	// Yボタン
#define DIXBOX_LB		DIJOFS_BUTTON(4);	// Lボタン
#define DIXBOX_RB		DIJOFS_BUTTON(5);	// Rボタン
#define DIXBOX_VIEW		DIJOFS_BUTTON(6);	// ビューボタン（中心左の小さいボタン）
#define DIXBOX_MENU		DIJOFS_BUTTON(7);	// メニューボタン（中心右の小さいボタン）
#define DIXBOX_LSTICK	DIJOFS_BUTTON(8);	// 左スティック押し込み
#define DIXBOX_RSTICK	DIJOFS_BUTTON(9);	// 右スティック押し込み
#define DIXBOX_XBOX		DIJOFS_BUTTON(10);	// XBoxボタン
#define DIXBOX_SHARE	DIJOFS_BUTTON(11);	// シェアボタン（中心下の小さいボタン）
#define DIXBOX_LT							// LTボタン（未実装）
#define DIXBOX_RT							// RTボタン（未実装）

// スティックの最大値
#define DIXBOX_STICK_MAXVALUE 65535L

#pragma endregion


namespace LWP::Base {
	class WinApp;
}

namespace LWP::Input {
	/// <summary>
	/// コントローラークラス
	/// </summary>
	class Controller final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Base::WinApp* winApp, IDirectInput8* directInput);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		// キーが押されていない場合 -> true
		bool None(uint8_t keyID);
		// キーが押された瞬間の場合 -> true
		bool Trigger(uint8_t keyID);
		// キーが押されている場合 -> true
		bool Press(uint8_t keyID);
		// キーが離された瞬間の場合 -> true
		bool Release(uint8_t keyID);

		/// <summary>
		/// Lスティックの入力を受け取る関数
		///   -Y　
		/// -X  +X
		///   +Y
		/// </summary>
		Math::Vector2 GetLStick();
		/// <summary>
		/// Rスティックの入力を受け取る関数
		///   -Y　
		/// -X  +X
		///   +Y
		/// </summary>
		Math::Vector2 GetRStick();

		/// <summary>
		/// 十字キーの入力検知
		///   -Y　
		/// -X  +X
		///   +Y
		/// </summary>
		Math::Vector2 GetCrossKey();


		/// <summary>
		/// デッドゾーンを設定
		/// </summary>
		void SetDeadZone(float value) { deadZone_ = value; }


	private: // ** メンバ変数 ** //

		// キーボードオブジェクト
		Microsoft::WRL::ComPtr<IDirectInputDevice8> controller_;

		// 入力状態
		DIJOYSTATE preStates_{};
		DIJOYSTATE states_{};

		// デッドゾーンの範囲
		float deadZone_ = 0.2f;
	};
}