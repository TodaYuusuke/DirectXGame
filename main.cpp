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

		MyEngine::DrawTriangle();

		MyEngine::EndFrame();
	}
	
	MyEngine::Finalize();

	return 0;
}