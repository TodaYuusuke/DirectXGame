#include "WinApp.h"
#include <cstdint>
#include "../utility/MyUtility.h"

#include <ShellScalingApi.h>
#pragma comment(lib,"Shcore.lib")

using namespace LWP::Base;
using namespace LWP::Utility;

void WinApp::Init(std::wstring title, int width, int height) {

	// 初期化
	wc_ = {};
	// ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	// ウィンドウクラス名(なんでもいい) ... const charをLPCWSTRに変換できないのので一度放置
	wc_.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc_);

	// タイトル
	title_ = title;

	// ウィンドウスタイルを用意
	windowStyle_ = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	// クライアント領域のサイズ
	clientWidth_ = (int32_t)width;
	clientHeight_ = (int32_t)height;

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,clientWidth_,clientHeight_ };

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,				// 利用するクラス名
		title_.c_str(),	// タイトルバーの文字（何でもいい）
		WS_OVERLAPPEDWINDOW,			// よく見るウィンドウスタイル
		CW_USEDEFAULT,					// 表示X座標（Windowsに任せる）
		CW_USEDEFAULT,					// 表示Y座標（WindowsOSに任せる）
		wrc.right - wrc.left,			// ウィンドウ横幅
		wrc.bottom - wrc.top,			// ウィンドウ縦幅
		nullptr,						// 親ウィンドウハンドル
		nullptr,						// メニューハンドル
		wc_.hInstance,					// インスタンスハンドル
		nullptr);						// オプション

	// ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

bool WinApp::ProcessMessage() {
	MSG msg{};
	if (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) // 終了メッセージが来たらループを抜ける
		{
			return true;
		}
	}

	return false;
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
		case WM_DESTROY:
			// OSに対して、アプリの終了を伝える
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::ChangeWindowMode() {
	if (currentMode == WindowMode::Window) { return; }
	currentMode = WindowMode::Window;

	// 通常ウィンドウに戻す
	SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);

	SetWindowPos(
		hwnd_, HWND_NOTOPMOST, windowRect_.left, windowRect_.top,
		windowRect_.right - windowRect_.left, windowRect_.bottom - windowRect_.top,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);

	ShowWindow(hwnd_, SW_NORMAL);
}
void WinApp::ChangeFullScreenMode() {
	if (currentMode == WindowMode::FullScreen) { return; }
	currentMode = WindowMode::FullScreen;

	// 元の状態を覚えておく
	GetWindowRect(hwnd_, &windowRect_);

	// 仮想フルスクリーン化
	SetWindowLong(
		hwnd_, GWL_STYLE,
		windowStyle_ &
		~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

	RECT fullscreenRect{ 0 };
	HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(info);
	GetMonitorInfo(monitor, &info);
	fullscreenRect.right = info.rcMonitor.right - info.rcMonitor.left;
	fullscreenRect.bottom = info.rcMonitor.bottom - info.rcMonitor.top;
	// ここのNOTOPMOSTで最前面固定を設定する
	SetWindowPos(
		hwnd_, HWND_NOTOPMOST, fullscreenRect.left, fullscreenRect.top, fullscreenRect.right,
		fullscreenRect.bottom, SWP_FRAMECHANGED | SWP_NOACTIVATE);
	ShowWindow(hwnd_, SW_MAXIMIZE);
}
void WinApp::ChangeBorderlessWindowMode() {
	//if (currentMode == WindowMode::BorderlessWindow) { return; }
	//currentMode = WindowMode::BorderlessWindow;

	//// 元の状態を覚えておく
	//GetWindowRect(hwnd_, &windowRect_);

	//// 通常ウィンドウに戻す
	//SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
	//// タイトルバーのないウィンドウにする
	////SetWindowLong(hwnd_, GWL_STYLE, WS_POPUP | WS_BORDER);

	//SetWindowPos(
	//	hwnd_, HWND_NOTOPMOST, 0, 0,
	//	windowRect_.right, windowRect_.bottom,
	//	SWP_FRAMECHANGED | SWP_NOACTIVATE);

	//ShowWindow(hwnd_, SW_NORMAL);
}

int WinApp::GetScaleFactor() {
	int scale = -1;
	
	// プロセス番号を取得
	//DWORD processId;
	//GetWindowThreadProcessId(hwnd_, &processId);
	//HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

	PROCESS_DPI_AWARENESS awareness;
	if (GetProcessDpiAwareness(NULL, &awareness) == S_OK) {
		if (awareness == PROCESS_SYSTEM_DPI_AWARE) {
			// System DPI aware
			scale = 100;
		}
		else if (awareness == PROCESS_PER_MONITOR_DPI_AWARE) {
			// Per-monitor DPI aware
			UINT dpiX, dpiY;
			if (SUCCEEDED(GetDpiForMonitor(MonitorFromWindow(GetConsoleWindow(), MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
				scale = static_cast<int>(dpiX * 100.0 / 96.0 + 0.5);
			}
		}
	}
	return scale;
}