#pragma once
#include <Windows.h>
#include <cstdint>
#include "../../Externals/imgui/imgui.h"
#include "../../Externals/imgui/imgui_impl_dx12.h"
#include "../../Externals/imgui/imgui_impl_win32.h"

#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
		void Initialize(std::wstring title, int width, int height);

		/// <summary>
		/// ウィンドウからのイベントをチェックする関数
		/// </summary>
		/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
		bool ProcessMessage();

		/// <summary>
		/// ウィンドウモードにする関数
		/// </summary>
		void ChangeWindowMode();
		/// <summary>
		/// ウィンドウモードにする関数
		/// </summary>
		void ChangeFullScreenMode();
		/// <summary>
		/// ウィンドウモードにする関数
		/// </summary>
		void ChangeBorderlessWindowMode();

		/// <summary>
		/// 画面の拡大率を取得する関数
		/// </summary>
		/// <returns></returns>
		int GetScaleFactor();

		// アクセサ
		int GetClientWidth() { return clientWidth_; }
		int GetClientHeight() { return clientHeight_; }
		WNDCLASS GetWNDCLASS() const { return wc_; }
		HWND GetHWND() const { return hwnd_; }

	private: // メンバ変数

		// タイトル
		std::wstring title_;

		// クライアント領域のサイズ
		int32_t clientWidth_;
		int32_t clientHeight_;


		// ウィンドウクラスの属性をまとめた構造体
		WNDCLASS wc_;
		// ウィンドウ
		HWND hwnd_;

		// 現在のウィンドウモード
		enum class WindowMode {
			Window = 0,
			FullScreen = 1,
			BorderlessWindow = 2,
		}currentMode = WindowMode::Window;

		// 元のウィンドウの情報を保持
		RECT windowRect_;
		// スタイル
		UINT windowStyle_;

	private: // 非公開のメンバ関数
		WinApp(const WinApp&) = delete;
		const WinApp& operator=(const WinApp&) = delete;

		/// <summary>
		/// ウィンドウプロシージャ
		/// </summary>
		static LRESULT CALLBACK WindowProc(HWND hwnd_, UINT msg, WPARAM wparam, LPARAM lparam);

	};
}