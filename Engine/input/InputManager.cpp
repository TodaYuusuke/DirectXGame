#include "InputManager.h"
#include "../base/WinApp.h"

#include <functional>
#include <string>

using namespace LWP::Base;
using namespace LWP::Input;
using namespace LWP::Math;

void Manager::Init() {
	// DirectInputの初期化
	HRESULT result = DirectInput8Create(
		WinApp::GetInstance()->GetWNDCLASS().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)directInput_.GetAddressOf(), nullptr);
	assert(SUCCEEDED(result));
	result;

	// キーボードデバイスの生成
	keyboard_.Initialize(WinApp::GetInstance(), directInput_.Get());
	// マウスデバイスの生成
	mouse_.Initialize(WinApp::GetInstance(), directInput_.Get());
	// コントローラーデバイスの生成
	for (int i = 0; i < XBOX_CONTROLLER_MAX; i++) {
		controller_[i].Initialize(i);
	}
}

void Manager::Update() {
	// デバイスの情報更新
	keyboard_.Update();
	mouse_.Update();
	for (int i = 0; i < XBOX_CONTROLLER_MAX; i++) {
		controller_[i].Update();
	}
}

void Manager::DebugGUI() {
	if (ImGui::BeginTabItem("Input")) {
		mouse_.DebugGUI();
		ImGui::EndTabItem();
	}
}
