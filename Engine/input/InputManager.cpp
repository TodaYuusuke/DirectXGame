#include "InputManager.h"
#include "../base/WinApp.h"

#include <functional>
#include <string>

using namespace LWP::Base;
using namespace LWP::Input;
using namespace LWP::Math;

void Manager::Initialize(WinApp* winApp) {
	winApp_ = winApp;

	// DirectInputの初期化
	HRESULT result = DirectInput8Create(
		winApp_->GetWNDCLASS().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)directInput_.GetAddressOf(), nullptr);
	assert(SUCCEEDED(result));
	result;

	// キーボードデバイスの生成
	keyboard_.Initialize(winApp_, directInput_.Get());
}

void Manager::Update() {
	// 新たなデバイスが接続されていないか検出しカウント
	UINT count = 0;
	directInput_->EnumDevices(
		DI8DEVTYPE_GAMEPAD,
		[](LPCDIDEVICEINSTANCEW ipddi, LPVOID pvRef) -> BOOL {
			ipddi;
			UINT* ptr = (UINT*)pvRef;
			*ptr = *ptr + 1;
			return DIENUM_CONTINUE;
		},
		&count,
		DIEDFL_ATTACHEDONLY
	);

	// もしカウントが1以上 かつ コントローラーが未登録なら初期化
	if (count > 0 && !isLoaded_) {
		controller_.Initialize(winApp_, directInput_.Get());
		isLoaded_ = true;
	}


	// デバイスの情報更新
	keyboard_.Update();
	if (isLoaded_) {
		controller_.Update();

		
	ImGui::Begin("Input");
	ImGui::Text("LStick x %f", controller_.GetLStick().x);
	ImGui::Text("LStick y %f", controller_.GetLStick().y);
	ImGui::Text("RStick x %f", controller_.GetRStick().x);
	ImGui::Text("RStick y %f", controller_.GetRStick().y);
	ImGui::Text("Cross x %f", controller_.GetCrossKey().x);
	ImGui::Text("Cross y %f", controller_.GetCrossKey().y);
	for (int i = 0; i < 32; i++) {
		ImGui::Text(("button[" + std::to_string(i) + "] is %d").c_str(), controller_.Press((uint8_t)i));
	}
	ImGui::End();
	}
}

bool Manager::GetKeyboardStateNone(uint8_t keyID) {
	return keyboard_.None(keyID);
}
bool Manager::GetKeyboardStateTrigger(uint8_t keyID) {
	return keyboard_.Trigger(keyID);
}
bool Manager::GetKeyboardStatePress(uint8_t keyID) {
	return keyboard_.Press(keyID);
}
bool Manager::GetKeyboardStateRelease(uint8_t keyID) {
	return keyboard_.Release(keyID);
}

bool Manager::GetControllerStateNone(uint8_t keyID) {
	return controller_.None(keyID);
}
bool Manager::GetControllerStateTrigger(uint8_t keyID) {
	return controller_.Trigger(keyID);
}
bool Manager::GetControllerStatePress(uint8_t keyID) {
	return controller_.Press(keyID);
}
bool Manager::GetControllerStateRelease(uint8_t keyID) {
	return controller_.Release(keyID);
}

Vector2 Manager::GetControllerLStick() {
	return controller_.GetLStick();
}
Vector2 Manager::GetControllerRStick() {
	return controller_.GetRStick();
}

Vector2 Manager::GetControllerCrossKey() {
	return controller_.GetCrossKey();
}