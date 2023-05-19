#include "MyEngine.h"
#include "MyUtility.h"
#include <format>

/// <summary>
/// ライブラリの初期化
/// </summary>
/// <param name="title">ウィンドウタイトル</param>
/// <param name="width">ウィンドウの横幅</param>
/// <param name="height">ウィンドウの縦幅</param>
void MyEngine::Initialize(const char* title, int width, int height) {

	// インスタンスを受け取る
	winApp_ = WinApp::GetInstance();
	directX_ = DirectX::GetInstance();
	drawSystem_ = DrawSystem::GetInstance();

	// 初期化
	winApp_->Initialize(title, width, height);
	directX_->Initialize(winApp_, width, height);
	drawSystem_->Initialize(directX_);
}

/// <summary>
/// ウィンドウからのイベントをチェックする関数
/// </summary>
/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
bool MyEngine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}


/// <summary>
/// フレーム開始
/// </summary>
void MyEngine::BeginFrame() {
	directX_->PreDraw();
}
/// <summary>
/// フレーム終了
/// </summary>
void MyEngine::EndFrame() {
	directX_->PostDraw();
}


/// <summary>
/// ライブラリの終了
/// </summary>
void MyEngine::Finalize() {

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

void MyEngine::DrawTriangle(Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color) {
	drawSystem_->DrawTriangle(pos1, pos2, pos3, color);
}

#pragma endregion


//ーーーーーーーーーーーーー//
//　　　　メンバ変数　　　　//
//ーーーーーーーーーーーーー//

WinApp* MyEngine::winApp_ = nullptr;
DirectX* MyEngine::directX_ = nullptr;
DrawSystem* MyEngine::drawSystem_ = nullptr;
