#pragma once
#include "MyEngine.h"

const char kWindowTitle[] = "CG2WindowClass";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyEngine::Initialize(kWindowTitle, 1280, 720);

	//////////////////////////////////////
	/////　　　　初期化処理ここまで　　　　/////
	//////////////////////////////////////

	
	// ウィンドウの×ボタンが押されるまでループ
	while (MyEngine::ProcessMessage()) {

		MyEngine::BeginFrame();

		// 左側の三角形を描画
		MyEngine::DrawTriangle(
			// 下
			{ -0.5f,-0.5f,0.0f },
			// 上
			{ -0.5f,0.5f,0.0f },
			// 右
			{ -0.2f,0.0f,0.0f },
			0xFFFFFFFF
		);
		// 右側の三角形を描画
		MyEngine::DrawTriangle(
			// 上
			{ 0.5f,0.5f,0.0f },
			// 下
			{ 0.5f,-0.5f,0.0f },
			// 左
			{ 0.2f,0.0f,0.0f },
			0xFFFFFFFF
		);
		

		MyEngine::EndFrame();
	}
	
	MyEngine::Finalize();

	return 0;
}