#pragma once
#include "keyboard/DIKeyboard.h"
#include "controller/XInputController.h"

// 前方宣言
namespace LWP::Base {
	class WinApp;
}

namespace LWP::Input {
	/// <summary>
	/// 入力デバイス管理クラス
	/// </summary>
	class Manager final {
	public: // ** パブリックなメンバ変数 ** //

		// キーボードオブジェクト
		DIKeyboard keyboard;

		// コントローラーオブジェクト
		//DIController controller_;
		XInput::Controller controller[XBOX_CONTROLLER_MAX];


	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Base::WinApp* winApp);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();


	private: // ** メンバ変数 ** //
		// ウィンドウ管理クラスのポインタ
		Base::WinApp* winApp_ = nullptr;

		// DirectInputオブジェクトの生成
		Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	};
}