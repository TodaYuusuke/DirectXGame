#include "XInputController.h"
#include "base/ImGuiManager.h"

#include <algorithm>

using namespace LWP::Input::XInput;
using namespace LWP::Math;

void Controller::Initialize(DWORD controllerNumber) {
	num_ = controllerNumber;
}

void Controller::Update() {
	// 前フレームの全キー入力の状態を保存する
	preState_ = state_;
	preLTFlag_ = LTFlag_;
	preRTFlag_ = RTFlag_;
	// 状態のデータを取得
	DWORD result = XInputGetState(num_, &state_);
	LTFlag_ = GetLTValue() > triggerDeadZone_;
	RTFlag_ = GetRTValue() > triggerDeadZone_;

	// コントローラーが接続されている
	if (result == ERROR_SUCCESS) {
		// 操作が更新されていない場合 -> afkFrameCount_を増加
		if (preState_.dwPacketNumber == state_.dwPacketNumber) {
			afkFrameCount_++;
		}
		else {
			afkFrameCount_ = 0;
		}
	}
	// コントローラーが接続されていない
	else {
		// データを空にしておく
		preState_.Gamepad = XINPUT_GAMEPAD();
		state_.Gamepad = XINPUT_GAMEPAD();
		preLTFlag_ = false;
		LTFlag_ = false;
		preRTFlag_ = false;
		RTFlag_ = false;
		// カウントは進める
		afkFrameCount_++;
	}
	
	// stateの中身チェック用コード
	//ImGui::Begin("XInputController");
	//ImGui::Text("buttons %d", state_.Gamepad.wButtons);
	//ImGui::Text("bLeftTrigger %f", GetLT());
	//ImGui::Text("bRightTrigger %f", GetRT());
	//ImGui::Text("LStick x,%f y,%f", GetLStick().x, GetLStick().y);
	//ImGui::Text("RStick x,%f y,%f", GetRStick().x, GetRStick().y);
	//ImGui::Text("afkFrameCount %d", afkFrameCount_);
	//ImGui::End();

}


bool Controller::None(int keyID) {
	if (keyID == XBOX_LT) { return !LTFlag_; }
	if (keyID == XBOX_RT) { return !RTFlag_; }
	return !(state_.Gamepad.wButtons & keyID);
}
bool Controller::Trigger(int keyID) {
	if (keyID == XBOX_LT) { return !preLTFlag_ && LTFlag_; }
	if (keyID == XBOX_RT) { return !preRTFlag_ && RTFlag_; }
	return !(preState_.Gamepad.wButtons & keyID) && (state_.Gamepad.wButtons & keyID);
}
bool Controller::Press(int keyID) {
	if (keyID == XBOX_LT) { return LTFlag_; }
	if (keyID == XBOX_RT) { return RTFlag_; }
	return (state_.Gamepad.wButtons & keyID);
}
bool Controller::Release(int keyID) {
	if (keyID == XBOX_LT) { return preLTFlag_ && !LTFlag_; }
	if (keyID == XBOX_RT) { return preRTFlag_ && !RTFlag_; }
	return (preState_.Gamepad.wButtons & keyID) && !(state_.Gamepad.wButtons & keyID);
}

float Controller::GetLTValue() {
	return static_cast<float>(state_.Gamepad.bLeftTrigger) / XBOX_TRIGGER_MAXVALUE;
}
float Controller::GetRTValue() {
	return static_cast<float>(state_.Gamepad.bRightTrigger) / XBOX_TRIGGER_MAXVALUE;
}

Vector2 Controller::GetLStick() {
	Vector2 vector = { 0.0f,0.0f };
	vector.x = static_cast<float>(state_.Gamepad.sThumbLX) / XBOX_STICK_MAXVALUE;
	vector.y = static_cast<float>(state_.Gamepad.sThumbLY) / XBOX_STICK_MAXVALUE;
	vector.x = std::clamp<float>(((std::abs(vector.x) < deadZone_) ? 0.0f : vector.x), -1.0f, 1.0f);
	vector.y = std::clamp<float>(((std::abs(vector.y) < deadZone_) ? 0.0f : vector.y), -1.0f, 1.0f);
	return vector;
}
Vector2 Controller::GetRStick() {
	Vector2 vector = { 0.0f,0.0f };
	vector.x = static_cast<float>(state_.Gamepad.sThumbRX) / XBOX_STICK_MAXVALUE;
	vector.y = static_cast<float>(state_.Gamepad.sThumbRY) / XBOX_STICK_MAXVALUE;
	vector.x = std::clamp<float>(((std::abs(vector.x) < deadZone_) ? 0.0f : vector.x), -1.0f, 1.0f);
	vector.y = std::clamp<float>(((std::abs(vector.y) < deadZone_) ? 0.0f : vector.y), -1.0f, 1.0f);
	return vector;
}

void Controller::SetVibration(float bigVibrationPower, float smallVibrationPower) {
	XINPUT_VIBRATION vibration = {
		static_cast<WORD>(bigVibrationPower * XBOX_MOTOR_MAXVALUE),
		static_cast<WORD>(smallVibrationPower * XBOX_MOTOR_MAXVALUE)
	};
	XInputSetState(num_, &vibration);
}
