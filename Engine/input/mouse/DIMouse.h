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
#include "math/vector/Vector2.h"

#pragma region 前方宣言
namespace LWP::Base {
	class WinApp;
}
#pragma endregion


/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
#define DIM_LEFT	0	// 左クリック
#define DIM_RIGHT	1	// 右クリック
#define DIM_MIDDLE	2	// ホイール押し込み
#define DIM_X0		3	// サイドキー0
#define DIM_X1		4	// サイドキー1
#define DIM_X2		5	// サイドキー2
#define DIM_X3		6	// サイドキー3
#define DIM_X4		7	// サイドキー4
#define DIM_COUNT	8	// 数

namespace LWP::Input {
	/// <summary>
	/// マウスクラス
	/// </summary>
	class DIMouse final {
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

		// マウスのスクリーン座標を取得
		Math::Vector2 GetPosition() { return position_; }
		// マウスの移動量を取得
		Math::Vector2 GetMove() { return position_ - prePosition_; }

		/// <summary>
		/// Debug用のGUI
		/// </summary>
		void DebugGUI();


	private: // ** メンバ変数 ** //

		// マウス座標の取得にHWNDが必要なため保存しておく
		Base::WinApp* winApp_ = nullptr;

		// マウスオブジェクト
		Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_ = nullptr;

		// マウス状態
		DIMOUSESTATE2 state_;
		DIMOUSESTATE2 preState_;
		
		// マウス座標
		Math::Vector2 position_ = { 0.0f,0.0f };
		Math::Vector2 prePosition_ = { 0.0f,0.0f };


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// DebugGUI用にキーのステータスを返す関数
		/// </summary>
		std::string GetKeyStateString(uint8_t keyID);
	};
}