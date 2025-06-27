#include "LWP.h"
#include "Config.h"
#include "component/Window.h"
#include "component/Information.h"

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
	memoryLeakChecker_ = std::make_unique<D3DResourceLeakChecker>();	// メモリリークチェック
	Initialize(str.c_str(), kWind::kResolutionWidth, kWind::kResolutionHeight);
}

void Engine::Run(IScene* firstScene) {
	// Sceneはここで生成
	sceneManager_ = std::make_unique<Scene::Manager>();
	sceneManager_->Init(firstScene);	// Scene初期化
	// デバッグカメラ用意
	debugCamera_ = std::make_unique<Information::DebugCamera>();
	debugCamera_->Init();

	// ウィンドウの×ボタンが押されるまで もしくは　End関数が呼ばれるまでループ
	while (ProcessMessage()) {
		BeginFrame();

		// 更新処理
		sceneManager_->Update();	// シーンの更新処理（当たり判定の登録もここで行う）
		debugCamera_->Update();	// デバッグカメラ更新はここで
		object_->Update();	// オブジェクトの更新処理

		// デバッグカメラのOnOffでレンダリングに使うカメラを切り替える
		/*directXCommon_->SetMainCamera(
			debugCamera_->GetActiveFlag() ?
			debugCamera_->GetCamera() :
			sceneManager_->GetMainCamera()
		);*/

		resource_->Update();	// リソース更新（アニメーションの更新処理）
		primitive_->Update();
		collision_->Update();	// 当たり判定検証

		// Primitiveの描画処理
		primitive_->Draw();

		EndFrame();
	}
	Finalize();

	// COMの終了処理
	CoUninitialize();
}

void Engine::InitializeForScene() {
	// Resource
	//resourceManager_->Initialize();
}

void Engine::Initialize(std::wstring title, int width, int height) {
	// インスタンスを受け取る //
	
	
	// Base
	WinApp::Create();
	winApp_ = WinApp::GetInstance();
	// デバイス生成
	GPUDevice::Create();
	// ヒープ生成
	RTV::Create();
	SRV::Create();
	DSV::Create();
	// DirectXコンパイラ生成
	DXC::Create();
	// DirectXCommon生成
	DirectXCommon::Create();
	directXCommon_ = DirectXCommon::GetInstance();
	imGuiManager_ = std::make_unique<ImGuiManager>();
	// Input
	Input::Manager::Create();
	input_ = Input::Manager::GetInstance();
	// Object
	Object::Manager::Create();
	object_ = Object::Manager::GetInstance();
	// Collider
	Object::CollisionManager::Create();
	collision_ = Object::CollisionManager::GetInstance();
	// Primitive
	Primitive::Manager::Create();
	primitive_ = Primitive::Manager::GetInstance();
	// Resource
	Resource::Manager::Create();
	resource_ = Resource::Manager::GetInstance();
	// Resource
	Information::FrameTracker::Create();
	frameTracker_ = Information::FrameTracker::GetInstance();

	// 初期化
	
	// Base
	winApp_->Init(title, width, height);
	directXCommon_->Init();
	imGuiManager_->Init();
	// Input
	input_->Init();
	// Object
	object_->Init();
	// Collider
	collision_->Init();
	// Primitive
	primitive_->Init();
	// Resource
	resource_->Init();
	// フレームトラッカー
	frameTracker_->Initialize();
}

bool Engine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}

void Engine::BeginFrame() {
	// タイマー計測開始
	frameTracker_->Start();

	imGuiManager_->Begin();
	input_->Update();
}

void Engine::EndFrame() {
	DebugGUI();

	directXCommon_->DrawCall();

	// 計測終了
	frameTracker_->End();
}

void Engine::DebugGUI() {
	// フラグがfalseなら早期リターン
	if (!Info::isDebugGUIVisible) { return; }

	ImGui::Begin("LWP");
	if (ImGui::BeginTabBar("LWP")) {
		primitive_->DebugGUI();
		object_->DebugGUI();
		collision_->DebugGUI();
		resource_->DebugGUI();
		directXCommon_->DebugGUI();
		sceneManager_->DebugGUI();
		frameTracker_->DebugGUI();

		if (ImGui::BeginTabItem("Info")) {
			ImGui::Text("---------------------------");
			if (ImGui::Button("Change Window Mode")) { Window::ChangeWindowMode(); }
			if (ImGui::Button("Change FullScreen Mode")) { Window::ChangeFullScreenMode(); }
			//if (ImGui::Button("Change BorderlessWindow Mode")) { Window::ChangeBorderlessWindowMode(); }
			ImGui::Text("---------------------------");
			ImGui::EndTabItem();
		}

		debugCamera_->DebugGUI();
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