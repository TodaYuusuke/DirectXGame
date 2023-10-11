#include "InputManager.h"
#include "../base/WinApp.h"

using namespace LWP::Base;
using namespace LWP::Input;

void Manager::Initialize(WinApp* winApp) {
	// DirectInputの初期化
	HRESULT result = DirectInput8Create(
		winApp->GetWNDCLASS().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)directInput.GetAddressOf(), nullptr);
	assert(SUCCEEDED(result));
	result;

	// キーボードデバイスの生成
	keyboard_.Initialize(winApp, directInput.Get());
}

void Manager::Update() {
	// デバイスの情報更新
	keyboard_.Update();
}

bool Manager::GetKeyStateNone(uint8_t keyID) {
	return keyboard_.None(keyID);
}
bool Manager::GetKeyStateTrigger(uint8_t keyID) {
	return keyboard_.Trigger(keyID);
}
bool Manager::GetKeyStatePress(uint8_t keyID) {
	return keyboard_.Press(keyID);
}
bool Manager::GetKeyStateRelease(uint8_t keyID) {
	return keyboard_.Release(keyID);
}