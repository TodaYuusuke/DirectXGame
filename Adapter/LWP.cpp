#include "LWP.h"
#include "Config.h"

using namespace LWP::System;

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Scene;

namespace kWind = LWP::Config::Window;

void Engine::StartUp(std::wstring str) {
	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	memoryLeakChecker_ = std::make_unique<D3DResourceLeakChecker>();
	Initialize(str.c_str(), kWind::kResolutionWidth, kWind::kResolutionHeight);
}

void Engine::Run(IScene* firstScene) {
	// Sceneはここで生成
	sceneManager_ = std::make_unique<Scene::Manager>();
	sceneManager_->Initialize(firstScene);	// Scene初期化
	// デバッグカメラ用意
	debugCamera_ = std::make_unique<Information::DebugCamera>();
	debugCamera_->Init();

	// ウィンドウの×ボタンが押されるまで もしくは　End関数が呼ばれるまでループ
	while (ProcessMessage()) {
		BeginFrame();

		// 更新処理
		sceneManager_->Update();	// シーンの更新処理（当たり判定の登録もここで行う）
		debugCamera_->Update();	// デバッグカメラ更新はここで
		objectManager_->Update(directXCommon_->GetRendererManager());	// 描画に必要なデータをRendererManagerに登録している（レンダーターゲットの登録もここで行っている）

		// デバッグカメラのOnOffでレンダリングに使うカメラを切り替える
		directXCommon_->SetMainCamera(
			debugCamera_->GetActiveFlag() ?
			debugCamera_->GetCamera() :
			sceneManager_->GetMainCamera()
		);

		resourceManager_->Update();	// リソース更新（アニメーションの更新処理）
		primitiveManager_->Update();

		collisionManager_->Update();	// 当たり判定検証
		// Primitiveの描画処理
		primitiveManager_->Draw(directXCommon_->GetRendererManager());

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
	collisionManager_->Initialize();
	// Resource
	//resourceManager_->Initialize();
}

void Engine::Initialize(std::wstring title, int width, int height) {
	// インスタンスを受け取る //
	
	// Base
	winApp_ = std::make_unique<WinApp>();
	directXCommon_ = std::make_unique<DirectXCommon>();
	imGuiManager_ = std::make_unique<ImGuiManager>();
	// Input
	inputManager_ = std::make_unique<Input::Manager>();
	// Object
	objectManager_ = std::make_unique<Object::Manager>();
	// Collider
	collisionManager_ = std::make_unique<Object::CollisionManager>();
	// Primitive
	primitiveManager_ = std::make_unique<Primitive::Manager>();
	// Resource
	resourceManager_ = std::make_unique<Resource::Manager>();

	// 初期化
	
	// Base
	winApp_->Initialize(title, width, height);
	directXCommon_->Initialize(winApp_.get());
	imGuiManager_->Initialize(winApp_.get(), directXCommon_.get());
	// Input
	inputManager_->Initialize(winApp_.get());
	// Object
	objectManager_->Initialize();
	// Collider
	collisionManager_->Initialize();
	// Primitive
	primitiveManager_->Initialize();
	// Resource
	resourceManager_->Initialize();
	// フレームトラッカー
	debugTimer_.Initialize();
}

bool Engine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}

void Engine::BeginFrame() {
	// タイマー計測開始
	debugTimer_.Start();

	imGuiManager_->Begin();
	inputManager_->Update();
}

void Engine::EndFrame() {
	DebugGUI();

	directXCommon_->DrawCall();

	// 計測終了
	debugTimer_.End();
}

void Engine::DebugGUI() {
	// フラグがfalseなら早期リターン
	if (!isShowDebugGUI) { return; }

	ImGui::Begin("LWP");
	if (ImGui::BeginTabBar("LWP")) {
#if DEMO
		primitiveManager_->DebugGUI();
		objectManager_->DebugGUI();
		collisionManager_->DebugGUI();
		resourceManager_->DebugGUI();
		directXCommon_->DebugGUI();
		sceneManager_->DebugGUI();
		debugCamera_->DebugGUI(sceneManager_->GetMainCamera()->transform);
		debugTimer_.DebugGUI();
#else
		debugTimer_.DebugGUI();
#endif
		ImGui::EndTabBar();
	}
	ImGui::End();
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