#include "DIMouse.h"
#include "../../base/WinApp.h"


// KeyIDが有効な値かチェックする
#define IsValidKeyID(id) assert(DIM_LEFT <= id && id < DIM_COUNT && "Invalid keyID. Please check KeyID again.");

using namespace LWP::Input;

void DIMouse::Initialize(Base::WinApp* winApp, IDirectInput8* directInput) {
	winApp_ = winApp;

	// マウスデバイスの生成
	HRESULT result = directInput->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));

	// 入力デ―タ形式のセット
	result = mouse_->SetDataFormat(&c_dfDIMouse2);//標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	// DISCL_FOREGROUND		... 画面が手前にある場合のみ入力を受け付ける
	// DISCL_NONEXCLUSIVE	... デバイスをこのアプリだけで専有しない
	result = mouse_->SetCooperativeLevel(
		winApp_->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

void DIMouse::Update() {
	// マウス情報の取得開始
	mouse_->Acquire();
	// 前フレームの入力の状態を保存する
	preState_ = state_;
	// マウスの入力状態を取得する
	mouse_->GetDeviceState(sizeof(state_), &state_);


	//マウス座標(スクリーン座標)を取得する
	POINT temp;
	//マウス座標(スクリーン座標)を取得する
	GetCursorPos(&temp);
	//クライアントエリア座標に変換する
	ScreenToClient(winApp_->GetHWND(), &temp);
	// 前フレームの座標を保存する
	prePosition_ = position_;
	// 座標を入力
	position_.x = float(temp.x);
	position_.y = float(temp.y);
}

bool DIMouse::None(uint8_t keyID) {
	IsValidKeyID(keyID);
	return !state_.rgbButtons[keyID];
}
bool DIMouse::Trigger(uint8_t keyID) {
	IsValidKeyID(keyID);
	return !preState_.rgbButtons[keyID] && state_.rgbButtons[keyID];
}
bool DIMouse::Press(uint8_t keyID) {
	IsValidKeyID(keyID);
	return state_.rgbButtons[keyID];
}
bool DIMouse::Release(uint8_t keyID) {
	IsValidKeyID(keyID);
	return preState_.rgbButtons[keyID] && !state_.rgbButtons[keyID];
}

void DIMouse::DebugGUI() {
	if (ImGui::TreeNode("Mouse")) {
		ImGui::Text("Position: (%.1f, %.1f)", position_.x, position_.y);
		ImGui::Text("Move: (%.1f, %.1f)", GetMove().x, GetMove().y);
		ImGui::Text("----- Key State -----");
		ImGui::Text("Left_Click ... %s", GetKeyStateString(DIM_LEFT).c_str());
		ImGui::Text("Right_Click ... %s", GetKeyStateString(DIM_RIGHT).c_str());
		ImGui::Text("Middle_Click ... %s", GetKeyStateString(DIM_MIDDLE).c_str());
		ImGui::Text("X0 ... %s", GetKeyStateString(DIM_X0).c_str());
		ImGui::Text("X1 ... %s", GetKeyStateString(DIM_X1).c_str());
		ImGui::Text("X2 ... %s", GetKeyStateString(DIM_X2).c_str());
		ImGui::Text("X3 ... %s", GetKeyStateString(DIM_X3).c_str());
		ImGui::Text("X4 ... %s", GetKeyStateString(DIM_X4).c_str());
		ImGui::TreePop();
	}
}

std::string DIMouse::GetKeyStateString(uint8_t keyID) {
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

