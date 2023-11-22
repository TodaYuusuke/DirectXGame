#include "Input.h"

using namespace LWP::System;
using namespace LWP;

bool Input::Keyboard::GetNone(uint8_t keyID) {
	return engine->inputManager_->keyboard.None(keyID);
}

bool Input::Keyboard::GetTrigger(uint8_t keyID) {
	return engine->inputManager_->keyboard.Trigger(keyID);
}

bool Input::Keyboard::GetPress(uint8_t keyID) {
	return engine->inputManager_->keyboard.Press(keyID);
}

bool Input::Keyboard::GetRelease(uint8_t keyID) {
	return engine->inputManager_->keyboard.Release(keyID);
}


bool Input::Controller::GetNone(int playerNum, int keyID) {
	return engine->inputManager_->controller[playerNum].None(keyID);
}
bool Input::Controller::GetTrigger(int playerNum, int keyID) {
	return engine->inputManager_->controller[playerNum].Trigger(keyID);
}
bool Input::Controller::GetPress(int playerNum, int keyID) {
	return engine->inputManager_->controller[playerNum].Press(keyID);
}
bool Input::Controller::GetRelease(int playerNum, int keyID) {
	return engine->inputManager_->controller[playerNum].Release(keyID);
}

float Input::Controller::GetLT(int playerNum) {
	return engine->inputManager_->controller[playerNum].GetLT();
}
float Input::Controller::GetRT(int playerNum) {
	return engine->inputManager_->controller[playerNum].GetRT();
}

Math::Vector2 Input::Controller::GetLStick(int playerNum) {
	return engine->inputManager_->controller[playerNum].GetLStick();
}
Math::Vector2 Input::Controller::GetRStick(int playerNum) {
	return engine->inputManager_->controller[playerNum].GetRStick();
}

void Input::Controller::SetDeadZone(int playerNum, float value) {
	engine->inputManager_->controller[playerNum].SetDeadZone(value);
}

void Input::Controller::SetVibration(int playerNum, float bigVibrationPower, float smallVibrationPower) {
	engine->inputManager_->controller[playerNum].SetVibration(bigVibrationPower, smallVibrationPower);
}