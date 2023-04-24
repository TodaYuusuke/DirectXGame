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

		// コマンドを積む
		commandList->RSSetViewports(1, &viewport);			// viewportを設定
		commandList->RSSetScissorRects(1, &scissorRect);	// Scirssorを設定
		// RootSignatureを設定。PSOに設定してるけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature);
		commandList->SetPipelineState(graphicsPipelineState);		// PSOを設定
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);	// VBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス、インスタンスについては今後
		commandList->DrawInstanced(3, 1, 0, 0);

		MyEngine::EndFrame();
	}
	
	MyEngine::Finalize();

	return 0;
}