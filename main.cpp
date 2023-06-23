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
	Vector3 leftTrianglePos[3] = {
		{ -0.5f,-0.5f,0.0f },
		{ -0.5f,0.5f,0.0f },
		{ -0.2f,0.0f,0.0f },
	};
	Vector3 rightTrianglePos[3] = {
		{ 0.5f,0.5f,0.0f },
		{ 0.5f,-0.5f,0.0f },
		{ 0.2f,0.0f,0.0f },
	};

	// ウィンドウの×ボタンが押されるまでループ
	while (MyEngine::ProcessMessage()) {

		MyEngine::BeginFrame();

		// 左側の三角形を描画
		MyEngine::DrawTriangle(
			leftTrianglePos[0],
			leftTrianglePos[1],
			leftTrianglePos[2],
			0x00FF00FF,
			DrawingProcessor::kFill
		);
		// 右側の三角形を描画
		MyEngine::DrawTriangle(
			rightTrianglePos[0],
			rightTrianglePos[1],
			rightTrianglePos[2],
			0x0000FFFF,
			DrawingProcessor::kWireFrame
		);
		

		MyEngine::EndFrame();
	}
	
	MyEngine::Finalize();

	return 0;
}