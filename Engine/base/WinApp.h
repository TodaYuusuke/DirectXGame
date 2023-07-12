#pragma once
#include <Windows.h>
#include <cstdint>

namespace LWP::Base {
	class WinApp
	{
	public: // メンバ関数

		// コンストラクタ
		WinApp() = default;
		// デストラクタ
		~WinApp() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(const char* title, int width, int height);

		/// <summary>
		/// ウィンドウからのイベントをチェックする関数
		/// </summary>
		/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
		bool ProcessMessage();


		// アクセサ
		int32_t GetClientWidth() const { return clientWidth_; }
		int32_t GetClientHeight() const { return clientHeight_; }
		HWND GetHWND() const { return hwnd_; }

	private: // メンバ変数

		// タイトル
		const char* title_;

		// クライアント領域のサイズ
		int32_t clientWidth_;
		int32_t clientHeight_;


		// ウィンドウクラスの属性をまとめた構造体
		WNDCLASS wc_;
		// ウィンドウ
		HWND hwnd_;


	private: // 非公開のメンバ関数
		WinApp(const WinApp&) = delete;
		const WinApp& operator=(const WinApp&) = delete;

		/// <summary>
		/// ウィンドウプロシージャ
		/// </summary>
		static LRESULT CALLBACK WindowProc(HWND hwnd_, UINT msg, WPARAM wparam, LPARAM lparam);

	};
}