#include "XInputController.h"
#include "base/ImGuiManager.h"

#include <algorithm>

// KeyIDが有効な値かチェックする
#define IsValidKeyID(id) assert(XINPUT_GAMEPAD_DPAD_UP <= id && id <= XBOX_RT && "Invalid keyID. Please check KeyID again.");

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
}

void Controller::ClearInput() {
	// 全入力データを初期化
	preState_ = {};
	state_ = {};
	afkFrameCount_ = 0;
	LTFlag_ = false;
	RTFlag_ = false;
	preLTFlag_ = false;
	preRTFlag_ = false;
}

bool Controller::None(int keyID) {
	IsValidKeyID(keyID);
	if (keyID == XBOX_LT) { return !LTFlag_; }
	if (keyID == XBOX_RT) { return !RTFlag_; }
	return !(state_.Gamepad.wButtons & keyID);
}
bool Controller::Trigger(int keyID) {
	IsValidKeyID(keyID);
	if (keyID == XBOX_LT) { return !preLTFlag_ && LTFlag_; }
	if (keyID == XBOX_RT) { return !preRTFlag_ && RTFlag_; }
	return !(preState_.Gamepad.wButtons & keyID) && (state_.Gamepad.wButtons & keyID);
}
bool Controller::Press(int keyID) {
	IsValidKeyID(keyID);
	if (keyID == XBOX_LT) { return LTFlag_; }
	if (keyID == XBOX_RT) { return RTFlag_; }
	return (state_.Gamepad.wButtons & keyID);
}
bool Controller::Release(int keyID) {
	IsValidKeyID(keyID);
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

void Controller::DebugGUI() {
	if (ImGui::TreeNode("Controller")) {
		ImGui::Text("A ... %s", GetKeyStateString(XBOX_A).c_str());
		ImGui::Text("B ... %s", GetKeyStateString(XBOX_B).c_str());
		ImGui::Text("X ... %s", GetKeyStateString(XBOX_X).c_str());
		ImGui::Text("Y ... %s", GetKeyStateString(XBOX_Y).c_str());
		ImGui::Text("LB ... %s", GetKeyStateString(XBOX_LB).c_str());
		ImGui::Text("RB ... %s", GetKeyStateString(XBOX_RB).c_str());
		ImGui::Text("----------------------");
		ImGui::Text("LSTICK Button ... %s", GetKeyStateString(XBOX_LSTICK).c_str());
		ImGui::Text("RSTICK Button ... %s", GetKeyStateString(XBOX_RSTICK).c_str());
		ImGui::Text("UP    ... %s", GetKeyStateString(XBOX_DPAD_UP).c_str());
		ImGui::Text("DOWN  ... %s", GetKeyStateString(XBOX_DPAD_DOWN).c_str());
		ImGui::Text("LEFT  ... %s", GetKeyStateString(XBOX_DPAD_LEFT).c_str());
		ImGui::Text("RIGHT ... %s", GetKeyStateString(XBOX_DPAD_RIGHT).c_str());
		ImGui::Text("----------------------");
		ImGui::Text("START ... %s", GetKeyStateString(XBOX_START).c_str());
		ImGui::Text("BACK  ... %s", GetKeyStateString(XBOX_BACK).c_str());
		ImGui::Text("----------------------");
		ImGui::Text("bLeftTrigger %f", GetLTValue());
		ImGui::Text("bRightTrigger %f", GetRTValue());
		ImGui::Text("LStick x,%f y,%f", GetLStick().x, GetLStick().y);
		ImGui::Text("RStick x,%f y,%f", GetRStick().x, GetRStick().y);
		ImGui::Text("afkFrameCount %d", afkFrameCount_);
		ImGui::TreePop();
	}
}

std::string Controller::GetKeyStateString(int keyID) {
	IsValidKeyID(keyID);

	if (Trigger(keyID)) {
		return "Trigger";
	}
	else if (Press(keyID)) {
		return "Press";
	}
	else if (Release(keyID)) {
		return "Release";
	}
	else if (None(keyID)) {
		return "None";
	}
	return "Error";
}