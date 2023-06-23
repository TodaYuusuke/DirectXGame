//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第１号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//

#pragma once

#include "WinApp.h"
#include "DirectXCommon.h"
#include "DrawingProcessor.h"

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



#pragma region 描画関数

	// 三角形描画関数
	static void DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color, DrawingProcessor::FillMode fillMode);

#pragma endregion


	//ーーーーーーーーーーーーー//
	//　　　　メンバ変数　　　　//
	//ーーーーーーーーーーーーー//
private:

	// ウィンドウ
	static WinApp* winApp_;
	// DirectX
	static DirectXCommon* directXCommon_;
	// 描画システム
	static DrawingProcessor* drawSystem_;
};