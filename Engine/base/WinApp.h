#pragma once
#include <Windows.h>
#include <cstdint>
#include "../../Externals/imgui/imgui.h"
#include "../../Externals/imgui/imgui_impl_dx12.h"
#include "../../Externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace LWP::Base {
	class WinApp
	{
	public: // メンバ関数

		// コンストラクタ
		WinApp() = default;
		// デストラクタ
		~WinApp() {
			wc_;
		};


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
		int GetClientWidth() { return clientWidth_; }
		int GetClientHeight() { return clientHeight_; }
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