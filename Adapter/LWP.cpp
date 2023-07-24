#include "LWP.h"
#include "../Engine/object/core/Camera.h"
#include "../Engine/scene/SceneManager.h"
#include "../Engine/utility/MyUtility.h"

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

using namespace LWP;

using namespace Base;
using namespace Math;
using namespace Object;
using namespace Primitive;
using namespace Scene;

void Engine::Run() {

	const char kWindowTitle[] = "CG2WindowClass";
	Initialize(kWindowTitle, 1280, 720);

	// ウィンドウの×ボタンが押されるまでループ
	while (ProcessMessage()) {
		BeginFrame();

		// 更新処理
		sceneManager_->Update();
		objectManager_->Update();
		// 描画処理
		sceneManager_->Draw();
		objectManager_->Draw();

		EndFrame();
	}
	Finalize();
}

void Engine::SetMainCamera(LWP::Object::Camera* mainCamera) { 
	primitiveManager_->SetwvpMatrix(mainCamera->GetwvpMatrixPtr());
}

void Engine::Initialize(const char* title, int width, int height) {
	// インスタンスを受け取る
	winApp_ = new WinApp();
	directXCommon_ = new DirectXCommon();
	objectManager_ = new Object::Manager();
	primitiveManager_ = new Primitive::Manager();
	sceneManager_ = new Scene::Manager();

	// 初期化
	winApp_->Initialize(title, width, height);

	directXCommon_->Initialize(winApp_, width, height);
	objectManager_->Initialize();
	primitiveManager_->Initialize(directXCommon_);
	sceneManager_->Initialize();
}

bool Engine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}


void Engine::BeginFrame() {
	directXCommon_->PreDraw();
}

void Engine::EndFrame() {
	directXCommon_->PostDraw();
	// 描画数リセット
	primitiveManager_->Reset();
}


void Engine::Finalize() {

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


// ウィンドウ
Base::WinApp* Engine::winApp_;
// DirectX
Base::DirectXCommon* Engine::directXCommon_;
// オブジェクト管理
Object::Manager* Engine::objectManager_;
// 描画システム
Primitive::Manager* Engine::primitiveManager_;

// シーンマネージャー
Scene::Manager* Engine::sceneManager_;