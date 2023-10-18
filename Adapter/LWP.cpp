#include "LWP.h"

using namespace LWP::System;

using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Scene;

void Engine::Run() {
	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	const char kWindowTitle[] = "CG2WindowClass";
	Initialize(kWindowTitle, 1280, 720);

	// ウィンドウの×ボタンが押されるまでループ
	while (ProcessMessage()) {
		BeginFrame();

		// 更新処理
		sceneManager_->Update();
		objectManager_->Update();
		//commandManager_->ImGui();
		// カメラのビュープロジェクションをcommandManagerに
		directXCommon_->GetCommandManager()->SetCameraViewProjection(sceneManager_->GetMainCamera());

		// 描画処理
		sceneManager_->Draw();
		objectManager_->Draw();
		primitiveManager_->Draw(directXCommon_->GetCommandManager());

		EndFrame();
	}
	Finalize();

	// COMの終了処理
	CoUninitialize();
}

void Engine::InitializeForScene() {
	// Object
	objectManager_->Initialize();
	// Primitive
	primitiveManager_->Initialize();
	// Resource
	//resourceManager_->Initialize();
}

void Engine::Initialize(const char* title, int width, int height) {
	// インスタンスを受け取る
	memoryLeakChecker_ = std::make_unique<D3DResourceLeakChecker>();
	// Base
	winApp_ = std::make_unique<WinApp>();
	directXCommon_ = std::make_unique<DirectXCommon>();
	imGuiManager_ = std::make_unique<ImGuiManager>();
	// Input
	inputManager_ = std::make_unique<Input::Manager>();
	// Object
	objectManager_ = std::make_unique<Object::Manager>();
	// Primitive
	primitiveManager_ = std::make_unique<Primitive::Manager>();
	// Resource
	resourceManager_ = std::make_unique<Resource::Manager>();
	// Scene
	sceneManager_ = std::make_unique<Scene::Manager>();

	// 初期化
	
	// Base
	winApp_->Initialize(title, width, height);
	directXCommon_->Initialize(winApp_.get() , width, height);
	imGuiManager_->Initialize(winApp_.get(), directXCommon_.get());
	// Input
	inputManager_->Initialize(winApp_.get());
	// Object
	objectManager_->Initialize();
	// Primitive
	primitiveManager_->Initialize();
	// Resource
	resourceManager_->Initialize();
	// Scene
	sceneManager_->Initialize();
}

bool Engine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}

void Engine::BeginFrame() {
#if _DEBUG //debug時
	// タイマー計測開始
	debugTimer_.Start();
#endif

	inputManager_->Update();
	directXCommon_->PreDraw();
	imGuiManager_->Begin();
}

void Engine::EndFrame() {
#if _DEBUG //debug時
	// FPS系の情報描画
	debugTimer_.DebugGUI();
#endif

	imGuiManager_->End();
	imGuiManager_->Draw();
	directXCommon_->PostDraw();

#if _DEBUG //debug時
	// 計測終了
	debugTimer_.End();
#endif
}

void Engine::Finalize() {

	CloseWindow(winApp_->GetHWND());

	// リソースリークチェック
	//IDXGIDebug1* debug;
	//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
	//	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	//	debug->Release();
	//}
}