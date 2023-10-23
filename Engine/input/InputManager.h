#pragma once
#include "keyboard/DIKeyboard.h"
#include "controller/DIController.h"

// 前方宣言
namespace LWP::Base {
	class WinApp;
}

namespace LWP::Input {
	/// <summary>
	/// 入力デバイス管理クラス
	/// </summary>
	class Manager final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Base::WinApp* winApp);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();


		// キーボード関連

		// キーが押されていない場合 -> true
		bool GetKeyboardStateNone(uint8_t keyID);
		// キーが押された瞬間の場合 -> true
		bool GetKeyboardStateTrigger(uint8_t keyID);
		// キーが押されている場合 -> true
		bool GetKeyboardStatePress(uint8_t keyID);
		// キーが離された瞬間の場合 -> true
		bool GetKeyboardStateRelease(uint8_t keyID);


		// コントローラー関連

		// キーが押されていない場合 -> true
		bool GetControllerStateNone(uint8_t keyID);
		// キーが押された瞬間の場合 -> true
		bool GetControllerStateTrigger(uint8_t keyID);
		// キーが押されている場合 -> true
		bool GetControllerStatePress(uint8_t keyID);
		// キーが離された瞬間の場合 -> true
		bool GetControllerStateRelease(uint8_t keyID);


	private: // ** メンバ変数 ** //
		// ウィンドウ管理クラスのポインタ
		Base::WinApp* winApp_ = nullptr;

		// DirectInputオブジェクトの生成
		Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
		
		// キーボードオブジェクト
		DIKeyboard keyboard_;

		// コントローラーオブジェクト
		DIController controller_;
		// コントローラー読み込み済みフラグ
		bool isLoaded_ = false;
	};
}