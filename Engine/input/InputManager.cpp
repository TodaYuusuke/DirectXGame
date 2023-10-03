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

	// キーボードデバイスの生成
	keyboard_.Initialize(winApp, directInput.Get());
}

void Manager::Update() {
	// デバイスの情報更新
	keyboard_.Update();
}