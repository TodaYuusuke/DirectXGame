#pragma once
#include "MyEngines/MyEngine.h"

const char kWindowTitle[] = "CG2WindowClass";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyEngine::Initialize(kWindowTitle, 1280, 720);

	//////////////////////////////////////
	/////　　　　初期化処理ここまで　　　　/////
	//////////////////////////////////////

	// 三角形の座標
	const Vector3 ktrianglePos[3] = {
		{ 0.0f,-1.0f,0.0f },
		{ 0.2f,-1.2f,0.0f },
		{ -0.2f,-1.2f,0.0f },
	};

	// ウィンドウの×ボタンが押されるまでループ
	while (MyEngine::ProcessMessage()) {

		MyEngine::BeginFrame();

		Vector3 translateY[3];
		memcpy(translateY, ktrianglePos, sizeof(translateY));
		// 三角形を描画
		for (int i = 0; i < 10; i++) {
			for (int index = 0; index < 3; index++) {
				translateY[index].y += 0.2f;
			}
			MyEngine::DrawTriangle(
				translateY[0],
				translateY[1],
				translateY[2],
				0xFFFFFFFF
			);
		}

		MyEngine::EndFrame();
	}
	
	MyEngine::Finalize();

	return 0;
}