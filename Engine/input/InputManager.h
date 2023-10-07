#pragma once
#include "keyboard/Keyboard.h"

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
		bool GetKeyStateNone(uint8_t keyID);
		// キーが押された瞬間の場合 -> true
		bool GetKeyStateTrigger(uint8_t keyID);
		// キーが押されている場合 -> true
		bool GetKeyStatePress(uint8_t keyID);
		// キーが離された瞬間の場合 -> true
		bool GetKeyStateRelease(uint8_t keyID);

	private: // ** メンバ変数 ** //

		// DirectInputオブジェクトの生成
		Microsoft::WRL::ComPtr<IDirectInput8> directInput;
		
		// キーボードオブジェクト
		Keyboard keyboard_;

	};
}