#include "Input.h"

using namespace LWP::System;
using namespace LWP;

bool Input::Keyboard::GetNone(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetKeyboard()->None(keyID);
}

bool Input::Keyboard::GetTrigger(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetKeyboard()->Trigger(keyID);
}

bool Input::Keyboard::GetPress(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetKeyboard()->Press(keyID);
}

bool Input::Keyboard::GetRelease(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetKeyboard()->Release(keyID);
}

bool Input::Mouse::GetNone(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetMouse()->None(keyID);
}
bool Input::Mouse::GetTrigger(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetMouse()->Trigger(keyID);
}
bool Input::Mouse::GetPress(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetMouse()->Press(keyID);
}
bool Input::Mouse::GetRelease(uint8_t keyID) {
	return Input::Manager::GetInstance()->GetMouse()->Release(keyID);
}
Math::Vector2 Input::Mouse::GetPosition() {
	return Input::Manager::GetInstance()->GetMouse()->GetPosition();
}
Math::Vector2 Input::Mouse::GetMove() {
	return Input::Manager::GetInstance()->GetMouse()->GetMove();
}


bool Input::Controller::GetNone(int keyID, int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->None(keyID);
}
bool Input::Controller::GetTrigger(int keyID, int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->Trigger(keyID);
}
bool Input::Controller::GetPress(int keyID, int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->Press(keyID);
}
bool Input::Controller::GetRelease(int keyID, int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->Release(keyID);
}

float Input::Controller::GetLT(int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->GetLTValue();
}
float Input::Controller::GetRT(int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->GetRTValue();
}

Math::Vector2 Input::Controller::GetLStick(int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->GetLStick();
}
Math::Vector2 Input::Controller::GetRStick(int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->GetRStick();
}

void Input::Controller::SetDeadZone(float value, int playerNum) {
	Input::Manager::GetInstance()->GetController(playerNum)->SetDeadZone(value);
}

void Input::Controller::SetVibration(float bigVibrationPower, float smallVibrationPower, int playerNum) {
	Input::Manager::GetInstance()->GetController(playerNum)->SetVibration(bigVibrationPower, smallVibrationPower);
}

UINT Input::Controller::GetAFKCount(int playerNum) {
	return Input::Manager::GetInstance()->GetController(playerNum)->GetAFKCount();
}
