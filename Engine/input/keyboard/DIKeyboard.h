#pragma once
#pragma region DirectInput

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#pragma endregion

#include <wrl.h>
#include <stdint.h>
#include <string>

#pragma region 前方宣言

namespace LWP::Base {
	class WinApp;
}

#pragma endregion


namespace LWP::Input {
	/// <summary>
	/// キーボードクラス
	/// </summary>
	class DIKeyboard final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Base::WinApp* winApp, IDirectInput8* directInput);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// キー入力情報を初期化
		/// </summary>
		void ClearInput();

		// キーが押されていない場合 -> true
		bool None(uint8_t keyID);
		// キーが押された瞬間の場合 -> true
		bool Trigger(uint8_t keyID);
		// キーが押されている場合 -> true
		bool Press(uint8_t keyID);
		// キーが離された瞬間の場合 -> true
		bool Release(uint8_t keyID);

		/// <summary>
		/// Debug用のGUI
		/// </summary>
		void DebugGUI();


	private: // ** メンバ変数 ** //

		// キーボードオブジェクト
		Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

		// キー入力状態
		BYTE preKeys_[256]{};
		BYTE keys_[256]{};
	};
}