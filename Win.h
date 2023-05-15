#pragma once
#include <Windows.h>
#include <string>

class Win
{
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Win* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const char* title, int width, int height);

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	static LRESULT CALLBACK WindowProc(HWND hwnd_, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// string -> wstringへの変換
	/// </summary>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// ウィンドウからのイベントをチェックする関数
	/// </summary>
	/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
	bool ProcessMessage();


	// アクセサ
	int32_t GetClientWidth() { return clientWidth_; }
	int32_t GetClientHeight() { return clientHeight_; }
	HWND GetHWND() { return hwnd_; }
	
private: // メンバ変数

	// タイトル
	const char* title_;

	// クライアント領域のサイズ
	int32_t clientWidth_;
	int32_t clientHeight_;
	
	
	// ウィンドウクラスの属性をまとめた構造体
	WNDCLASS wc_;
	// ウィンドウ
	HWND hwnd_
		;
};