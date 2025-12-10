#include "LWP.h"
#include "Config.h"
#include "component/Window.h"
#include "component/Information.h"
#include "component/Input.h"

using namespace LWP::System;

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Input;
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
		utility_->Update(); // 便利系の更新処理（タイマーなど）

		// デバッグカメラのOnOffでレンダリングに使うカメラを切り替える
		/*directXCommon_->SetMainCamera(
			debugCamera_->GetActiveFlag() ?
			debugCamera_->GetCamera() :
			sceneManager_->GetMainCamera()
		);*/

		resource_->Update();	// リソース更新（アニメーションの更新処理）
		primitive_->Update();
		collision_->Update();	// 当たり判定検証

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
	// Info
	Information::FrameTracker::Create();
	frameTracker_ = Information::FrameTracker::GetInstance();
	// Utility
	Utility::Manager::Create();
	utility_ = Utility::Manager::GetInstance();

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
	// Utility
	utility_->Init();
}

bool Engine::ProcessMessage() {
	return !winApp_->ProcessMessage();
}

void Engine::BeginFrame() {
	// タイマー計測開始
	frameTracker_->Start();

	imGuiManager_->Begin();
	input_->Update();

	// この段階でショートカットキーのチェックを行う
	DebugShortcutKey();
}

void Engine::EndFrame() {
	DebugGUI();
	ShowFPS();

	directXCommon_->DrawCall();

	// 計測終了
	frameTracker_->End();
}

void Engine::DebugShortcutKey() {
	// 基本のショートカット用ボタンをチェックする
	if (Keyboard::GetPress(DIK_RALT) && Keyboard::GetPress(DIK_L)) {
		
		// ウィンドウモード ... W
		if (Keyboard::GetTrigger(DIK_W)) { Window::ChangeWindowMode(); }
		// フルスクリーンモード ... F
		if (Keyboard::GetTrigger(DIK_F)) { Window::ChangeFullScreenMode(); }
		// ボーダーレスモード ... B
		//ImGui::Text("B ... Change BorderlessWindow Mode");
		//if (Keyboard::GetTrigger(DIK_B)) { Window::ChangeBorderlessWindowMode(); }

		// デバッグカメラ起動切り替え ... C
		if (Keyboard::GetTrigger(DIK_C)) { debugCamera_->ToggleEnable(); }

		// エンジンのImGui非表示切り替え ... D
		if (Keyboard::GetTrigger(DIK_D)) { Info::isDebugGUIVisible = !Info::isDebugGUIVisible; }
		// FPS表示切り替え ... P
		if (Keyboard::GetTrigger(DIK_P)) { Info::isShowFPS = !Info::isShowFPS; }
		// exe終了 ... Delete
		if (Keyboard::GetTrigger(DIK_DELETE)) { System::ShutDown(); }
	}
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
		input_->DebugGUI();
		directXCommon_->DebugGUI();
		sceneManager_->DebugGUI();
		//frameTracker_->DebugGUI();	// FPSは表示を別にしたいのでコメントアウト
		utility_->DebugGUI();

		if (ImGui::BeginTabItem("Info")) {
			ImGui::Text("Press [Right ALT] + [L] + any key to use the debug function");
			ImGui::Text("  W      ... Change Window Mode");
			ImGui::Text("  F      ... Change FullScreen Mode");
			ImGui::Text("  C      ... Toggle DebugCamera");
			ImGui::Text("  D      ... Toggle show LWP DebugGUI");
			ImGui::Text("  P      ... Toggle show FPS");
			ImGui::Text("  Delete ... Shutdown");
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
void Engine::ShowFPS() {
	// フラグがfalseなら早期リターン
	if (!Info::isShowFPS) { return; }

	ImVec4 fpsBgColor = ImVec4(0, 0, 0, 0.5f);
	ImVec4 fpsTextColor = ImVec4(1, 1, 1, 1);

	// ---- 位置設定：右上に固定 ----
	ImVec2 windowPos = ImVec2(0.0f, 0.0f);
	ImVec2 windowPivot = ImVec2(0.0f, 0.0f); // 右上アンカー

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(fpsBgColor.w);

	// ---- 色を適用 ----
	ImGui::PushStyleColor(ImGuiCol_WindowBg, fpsBgColor);
	ImGui::PushStyleColor(ImGuiCol_Text, fpsTextColor);

	// ---- ウィンドウフラグ ----
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove;

	ImGui::Begin("FPSWindow", nullptr, flags);
	frameTracker_->TablessDebugGUI();
	ImGui::Text("RightALT + L + P ... Toggle show FPS");
	ImGui::End();

	ImGui::PopStyleColor(2);
}
	

void Engine::Finalize() {
	// 各種解放
	imGuiManager_->Release();
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