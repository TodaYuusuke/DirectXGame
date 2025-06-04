#include "DIKeyboard.h"
#include "../../base/WinApp.h"

#include "utility/MyUtility.h"

// KeyIDが有効な値かチェックする
#define IsValidKeyID(id) assert(DIK_ESCAPE <= id && id <= DIK_MEDIASELECT && "Invalid keyID. Please check KeyID again.");

using namespace LWP::Input;

void DIKeyboard::Initialize(Base::WinApp* winApp, IDirectInput8* directInput) {
	// キーボードデバイスの生成
	HRESULT result = directInput->CreateDevice(GUID_SysKeyboard, keyboard_.GetAddressOf(), NULL);
	assert(SUCCEEDED(result));

	// 入力データの形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);	// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	// DISCL_FOREGROUND		... 画面が手前にある場合のみ入力を受け付ける
	// DISCL_NONEXCLUSIVE	... デバイスをこのアプリだけで専有しない
	// DISCL_NOWINKEY		... Windowsキーを無効にする
	result = keyboard_->SetCooperativeLevel(
		winApp->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
}

void DIKeyboard::Update() {
	// キーボード情報の取得開始
	keyboard_->Acquire();

	// 前フレームの全キー入力の状態を保存する
	memcpy(preKeys_, keys_, sizeof(keys_));

	// 全キーの入力状態を取得する
	if (FAILED(keyboard_->GetDeviceState(sizeof(keys_), keys_))) {
		// キー入力をクリアしておく
		ZeroMemory(keys_, sizeof(keys_));
	}
}

bool DIKeyboard::None(uint8_t keyID) {
	IsValidKeyID(keyID);
	return !keys_[keyID];
}
bool DIKeyboard::Trigger(uint8_t keyID) {
	IsValidKeyID(keyID);
	return !preKeys_[keyID] && keys_[keyID];
}
bool DIKeyboard::Press(uint8_t keyID) {
	IsValidKeyID(keyID);
	return keys_[keyID];
}
bool DIKeyboard::Release(uint8_t keyID) {
	IsValidKeyID(keyID);
	return preKeys_[keyID] && !keys_[keyID];
}