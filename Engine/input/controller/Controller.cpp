#include "Controller.h"
#include "../../base/WinApp.h"
#include "../../utility/MyUtility.h"


using namespace LWP::Input;
using namespace LWP::Math;

void Controller::Initialize(Base::WinApp* winApp, IDirectInput8* directInput) {
	// 1コントローラーデバイスの生成
	HRESULT result = directInput->CreateDevice(GUID_Joystick, controller_.GetAddressOf(), NULL);
	assert(SUCCEEDED(result));

	// 入力データの形式のセット
	result = controller_->SetDataFormat(&c_dfDIJoystick);	// 標準形式
	assert(SUCCEEDED(result));

	// 軸のモードを絶対軸に設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(DIPROPDWORD); // 使用される構造体のサイズ
	diprop.diph.dwHeaderSize = sizeof(DIPROPHEADER); // DIPROPHEADER構造体のサイズ
	diprop.diph.dwHow = DIPH_DEVICE; // 対象（ここではデバイス）
	diprop.diph.dwObj = 0; // デバイスを対象とする時はいつも0
	diprop.dwData = DIPROPAXISMODE_ABS; // 絶対値モードに設定
	controller_->SetProperty(DIPROP_AXISMODE, &diprop.diph); // 絶対軸に設定

	// 排他制御レベルのセット
	// DISCL_FOREGROUND		... 画面が手前にある場合のみ入力を受け付ける
	// DISCL_NONEXCLUSIVE	... デバイスをこのアプリだけで専有しない
	// DISCL_NOWINKEY		... Windowsキーを無効にする
	result = controller_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
}

void Controller::Update() {
	// キーボード情報の取得開始
	controller_->Acquire();

	// 前フレームの全キー入力の状態を保存する
	preStates_ = states_;

	// 全キーの入力状態を取得する
	controller_->GetDeviceState(sizeof(states_), &states_);
	
	// stateの中身チェック用コード
	/*
	ImGui::Begin("COnt");
	ImGui::Text("lx %d", states_.lX);
	ImGui::Text("ly %d", states_.lY);
	ImGui::Text("lz %d", states_.lZ);
	ImGui::Text("lRx %d", states_.lRx);
	ImGui::Text("lRy %d", states_.lRy);
	ImGui::Text("lRz %d", states_.lRz);
	ImGui::Text("rgbSlider[0] %d", states_.rglSlider[0]);
	ImGui::Text("rgbSlider[1] %d", states_.rglSlider[1]);
	ImGui::Text("rgdwPOV[0] %d", states_.rgdwPOV[0]);
	ImGui::Text("rgdwPOV[1] %d", states_.rgdwPOV[1]);
	ImGui::Text("rgdwPOV[2] %d", states_.rgdwPOV[2]);
	ImGui::Text("rgdwPOV[3] %d", states_.rgdwPOV[3]);
	for (int i = 0; i < 32; i++) {
		ImGui::Text(("button[" + std::to_string(i) + "] is %d").c_str(), states_.rgbButtons[i]);
	}
	ImGui::End();
	*/
}


bool Controller::None(uint8_t keyID) {
	return !states_.rgbButtons[keyID];
}
bool Controller::Trigger(uint8_t keyID) {
	return !preStates_.rgbButtons[keyID] && states_.rgbButtons[keyID];
}
bool Controller::Press(uint8_t keyID) {
	return states_.rgbButtons[keyID];
}
bool Controller::Release(uint8_t keyID) {
	return preStates_.rgbButtons[keyID] && !states_.rgbButtons[keyID];
}

Vector2 Controller::GetLStick() {
	Vector2 vector = { 0.0f,0.0f };
	vector.x = 2.0f * (static_cast<float>(states_.lX) / static_cast<float>(DIXBOX_STICK_MAXVALUE)) - 1.0f;
	vector.y = 2.0f * (static_cast<float>(states_.lY) / static_cast<float>(DIXBOX_STICK_MAXVALUE)) - 1.0f;
	vector.x = (std::abs(vector.x) < deadZone_) ? 0.0f : vector.x;
	vector.y = (std::abs(vector.y) < deadZone_) ? 0.0f : vector.y;
	return vector;
}
Vector2 Controller::GetRStick() {
	Vector2 vector = { 0.0f,0.0f };
	vector.x = 2.0f * (static_cast<float>(states_.lRx) / static_cast<float>(DIXBOX_STICK_MAXVALUE)) - 1.0f;
	vector.y = 2.0f * (static_cast<float>(states_.lRy) / static_cast<float>(DIXBOX_STICK_MAXVALUE)) - 1.0f;
	vector.x = (std::abs(vector.x) < deadZone_) ? 0.0f : vector.x;
	vector.y = (std::abs(vector.y) < deadZone_) ? 0.0f : vector.y;
	return vector;
}

Vector2 Controller::GetCrossKey() {
	if (states_.rgdwPOV[0] == -1) { return { 0.0f,0.0f }; }
	Vector2 vector = { 0.0f,-1.0f };
	return vector.Rotate(Utility::DegreeToRadian(static_cast<int>(states_.rgdwPOV[0] / 100.0f)));
}