//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第１号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//

#pragma once
#include <Windows.h>
#include <string>


class MyEngine
{
public: // メンバ関数

	/// <summary>
	/// ライブラリの初期化
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="width">ウィンドウの横幅</param>
	/// <param name="height">ウィンドウの縦幅</param>
	static void Initialize(const char* title, int width, int height);
	/// <summary>
	/// ウィンドウからのイベントをチェックする関数
	/// </summary>
	/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
	static bool ProcessMessage();


	/// <summary>
	/// フレーム開始
	/// </summary>
	static void BeginFrame();
	/// <summary>
	/// フレーム終了
	/// </summary>
	static void EndFrame();


	/// <summary>
	/// ライブラリの終了
	/// </summary>
	static void Finalize();



	//ーーーーーーーーーーーーーー//
	//　　　　その他の関数　　　　//
	//ーーーーーーーーーーーーーー//
private:

	//// ウィンドウプロシージャ
	//LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//// ログの表示
	//void Log(const std::string& message);

	//// string -> wstringへの変換
	//std::wstring ConvertString(const std::string& str);
	//// wstring -> stringへの変換
	//std::string ConvertString(const std::wstring& str);

	//// シェーダーのコンパイル関数
	//IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

};

