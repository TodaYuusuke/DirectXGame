#include "LWP.h"
#include "Config.h"

using namespace LWP::System;

using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Scene;

namespace kWind = LWP::Config::Window;

void Engine::Run(IScene* firstScene) {
	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	const char kWindowTitle[] = "Window Title";
	Initialize(kWindowTitle, kWind::kResolutionWidth, kWind::kResolutionHeight, firstScene);

	// ウィンドウの×ボタンが押されるまで もしくは　End関数が呼ばれるまでループ
	while (ProcessMessage()) {
		BeginFrame();

		// 更新処理
		sceneManager_->Update();
		objectManager_->Update(directXCommon_->GetCommandManager());	// 描画に必要なデータをCommandManagerに登録している
		primitiveManager_->Update();
		colliderManager_->Update();	// 当たり判定検証

		// カメラのビュープロジェクションをcommandManagerに
		directXCommon_->GetCommandManager()->SetMainRendering(sceneManager_->GetMainCamera());

		// 描画処理
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
	// Primitive
	colliderManager_->Initialize();
	// Resource
	//resourceManager_->Initialize();
}

void Engine::Initialize(const char* title, int width, int height, IScene* firstScene) {
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
	// Collider
	colliderManager_ = std::make_unique<Object::Collider::Manager>();
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
	// Collider
	colliderManager_->Initialize();
	// Primitive
	primitiveManager_->Initialize();
	// Resource
	resourceManager_->Initialize();
	// Scene
	sceneManager_->Initialize(firstScene);
	// フレームトラッカー
	debugTimer_.Initialize();
}

bool Engine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}

void Engine::BeginFrame() {
	// タイマー計測開始
	debugTimer_.Start();

	directXCommon_->PreDraw();
	imGuiManager_->Begin();
	inputManager_->Update();
}

void Engine::EndFrame() {
	// FPS系の情報描画
	if (isShowDebugInfo) { debugTimer_.DebugGUI(); }

	directXCommon_->DrawCall();
	directXCommon_->PostDraw();

	// 計測終了
	debugTimer_.End();
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