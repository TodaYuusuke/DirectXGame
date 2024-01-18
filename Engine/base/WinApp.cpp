#include "WinApp.h"
#include <cstdint>
#include "../utility/MyUtility.h"

using namespace LWP::Base;
using namespace LWP::Utility;

void WinApp::Initialize(const char* title, int width, int height) {

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
		ConvertString(title_).c_str(),	// タイトルバーの文字（何でもいい）
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
	// ウィンドウのスタイルを変更（WS_OVERLAPPEDWINDOWは適切なスタイルに置き換えてください）
	SetWindowLong(hwnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW);

	// ウィンドウを指定されたサイズと位置に移動
	RECT windowRect = { 0, 0, clientWidth_, clientHeight_ };  // ウィンドウの新しいサイズ
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int newWidth = windowRect.right - windowRect.left;
	int newHeight = windowRect.bottom - windowRect.top;

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - newWidth) / 2;  // 画面中央に配置
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - newHeight) / 2;

	MoveWindow(hwnd_, xPos, yPos, newWidth, newHeight, TRUE);
}
void WinApp::ChangeFullScreenMode() {
	SetWindowLong(hwnd_, GWL_STYLE, WS_VISIBLE | WS_POPUP);//ウィンドウのスタイルを変更
	MoveWindow(hwnd_, GetSystemMetrics(SM_XVIRTUALSCREEN),
		GetSystemMetrics(SM_YVIRTUALSCREEN),
		GetSystemMetrics(SM_CXVIRTUALSCREEN),
		GetSystemMetrics(SM_CYVIRTUALSCREEN), TRUE);
	//ウィンドウを画面に合わせる
}
void WinApp::ChangeBorderlessWindowMode() {
	// 未実装
}