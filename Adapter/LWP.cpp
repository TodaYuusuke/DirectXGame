#include "LWP.h"
#include "../Engine/utility/MyUtility.h"
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

/// <summary>
/// ライブラリの初期化
/// </summary>
/// <param name="title">ウィンドウタイトル</param>
/// <param name="width">ウィンドウの横幅</param>
/// <param name="height">ウィンドウの縦幅</param>
void LWP::Adapter::Initialize(const char* title, int width, int height) {

	// インスタンスを受け取る
	winApp_ = new Base::WinApp();
	directXCommon_ = new DirectXCommon();
	drawSystem_ = new DrawingProcessor();

	// 初期化
	winApp_->Initialize(title, width, height);
	directXCommon_->Initialize(winApp_, width, height);
	drawSystem_->Initialize(directXCommon_);
}

/// <summary>
/// ウィンドウからのイベントをチェックする関数
/// </summary>
/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
bool Adapter::ProcessMessage() {
	return !winApp_->ProcessMessage();
}


/// <summary>
/// フレーム開始
/// </summary>
void Adapter::BeginFrame() {
	directXCommon_->PreDraw();
}
/// <summary>
/// フレーム終了
/// </summary>
void Adapter::EndFrame() {
	directXCommon_->PostDraw();
	// 描画数リセット
	drawSystem_->Reset();
}


/// <summary>
/// ライブラリの終了
/// </summary>
void Adapter::Finalize() {

	CloseWindow(winApp_->GetHWND());

	//// リソースリークチェック
	//IDXGIDebug1* debug;
	//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
	//	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	//	debug->Release();
	//}
}



#pragma region 描画関数

void Adapter::DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color, DrawingProcessor::FillMode fillMode) {
	drawSystem_->DrawTriangle(pos1, pos2, pos3, color, fillMode);
}

#pragma endregion


//ーーーーーーーーーーーーー//
//　　　　メンバ変数　　　　//
//ーーーーーーーーーーーーー//

WinApp* Adapter::winApp_ = nullptr;
DirectXCommon* Adapter::directXCommon_ = nullptr;
DrawingProcessor* Adapter::drawSystem_ = nullptr;
