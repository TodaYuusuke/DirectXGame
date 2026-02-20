#pragma once
#include "keyboard/DIKeyboard.h"
#include "mouse/DIMouse.h"
#include "controller/XInputController.h"

#include "utility/other/Singleton.h"

// 前方宣言
namespace LWP::Base {
	class WinApp;
}

namespace LWP::Input {
	/// <summary>
	/// 入力デバイス管理クラス
	/// </summary>
	class Manager : public Utility::ISingleton<Manager> {
		friend class Utility::ISingleton<Manager>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		Manager() = default;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// 入力機器の情報を初期化
		/// </summary>
		void ClearInput();
		
		/// <summary>
		/// Debug用のGUI
		/// </summary>
		void DebugGUI();

		/// <summary>
		/// キーボードオブジェクトを取得
		/// </summary>
		DIKeyboard* GetKeyboard() { return &keyboard_; };
		/// <summary>
		/// マウスオブジェクトを取得
		/// </summary>
		DIMouse* GetMouse() { return &mouse_; };
		/// <summary>
		/// コントローラーオブジェクトを取得
		/// </summary>
		XInput::Controller* GetController(int playerNum) { return &controller_[playerNum]; }


	private: // ** メンバ変数 ** //
		// ウィンドウ管理クラスのポインタ
		Base::WinApp* winApp_ = nullptr;

		// DirectInputオブジェクトの生成
		Microsoft::WRL::ComPtr<IDirectInput8> directInput_;

		// キーボードオブジェクト
		DIKeyboard keyboard_;
		// マウスオブジェクト
		DIMouse mouse_;
		// コントローラーオブジェクト
		XInput::Controller controller_[XBOX_CONTROLLER_MAX];
	};
}