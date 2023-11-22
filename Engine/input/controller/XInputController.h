#pragma once
#include "../Engine/math/vector/Vector2.h"

#include <Windows.h>	// xinputのエラー回避

#include <xinput.h>
#pragma comment(lib, "xinput.lib")
//#pragma comment(lib, "xinput9_1_0.lib")


#pragma region KeyID

// 各ボタンID
#define XBOX_A		XINPUT_GAMEPAD_A	// Aボタン
#define XBOX_B		XINPUT_GAMEPAD_B	// Bボタン
#define XBOX_X		XINPUT_GAMEPAD_X	// Xボタン
#define XBOX_Y		XINPUT_GAMEPAD_Y	// Yボタン
#define XBOX_LB		XINPUT_GAMEPAD_LEFT_SHOULDER	// Lボタン
#define XBOX_RB		XINPUT_GAMEPAD_RIGHT_SHOULDER	// Rボタン
#define XBOX_START	XINPUT_GAMEPAD_START	// スタートボタン（）
#define XBOX_BACK	XINPUT_GAMEPAD_BACK		// バックボタン（）
#define XBOX_LSTICK	XINPUT_GAMEPAD_LEFT_THUMB	// 左スティック押し込み
#define XBOX_RSTICK	XINPUT_GAMEPAD_RIGHT_THUMB	// 右スティック押し込み
#define XBOX_DPAD_UP	XINPUT_GAMEPAD_DPAD_UP		// 十字キー上
#define XBOX_DPAD_DOWN	XINPUT_GAMEPAD_DPAD_DOWN	// 十字キー下
#define XBOX_DPAD_LEFT	XINPUT_GAMEPAD_DPAD_LEFT	// 十字キー左
#define XBOX_DPAD_RIGHT	XINPUT_GAMEPAD_DPAD_RIGHT	// 十字キー右

// トリガーの最大値
#define XBOX_TRIGGER_MAXVALUE 255
// スティックの最大値
#define XBOX_STICK_MAXVALUE 32767
// モーターの振動の最大値
#define XBOX_MOTOR_MAXVALUE 65535

// コントローラーの最大値
#define XBOX_CONTROLLER_MAX 4

#pragma endregion


namespace LWP::Base {
	class WinApp;
}

namespace LWP::Input::XInput {
	/// <summary>
	/// コントローラークラス
	/// </summary>
	class Controller final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DWORD controllerNumber);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		// キーが押されていない場合 -> true
		bool None(int keyID);
		// キーが押された瞬間の場合 -> true
		bool Trigger(int keyID);
		// キーが押されている場合 -> true
		bool Press(int keyID);
		// キーが離された瞬間の場合 -> true
		bool Release(int keyID);

		// Lトリガーの入力を受け取る関数
		float GetLT();
		// Rトリガーの入力を受け取る関数
		float GetRT();

		// Lスティックの入力を受け取る関数
		Math::Vector2 GetLStick();
		// Rスティックの入力を受け取る関数
		Math::Vector2 GetRStick();

		// デッドゾーンを設定
		void SetDeadZone(float value) { deadZone_ = value; }
		// 振動をセットする
		void SetVibration(float bigVibrationPower, float smallVibrationPower);

	private: // ** メンバ変数 ** //

		// 自分のコントローラー番号
		DWORD num_;

		// 入力状態
		XINPUT_STATE preState_{};
		XINPUT_STATE state_{};

		// 無操作フレームカウント
		UINT afkFrameCount_ = 0;

		// デッドゾーンの範囲
		float deadZone_ = 0.2f;
	};
}