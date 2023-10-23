#include "Input.h"

using namespace LWP::System;
using namespace LWP;

bool Input::Keyboard::GetNone(uint8_t keyID) {
	return engine->inputManager_->GetKeyboardStateNone(keyID);
}

bool Input::Keyboard::GetTrigger(uint8_t keyID) {
	return engine->inputManager_->GetKeyboardStateTrigger(keyID);
}

bool Input::Keyboard::GetPress(uint8_t keyID) {
	return engine->inputManager_->GetKeyboardStatePress(keyID);
}

bool Input::Keyboard::GetRelease(uint8_t keyID) {
	return engine->inputManager_->GetKeyboardStateRelease(keyID);
}


bool Input::Controller::GetNone(uint8_t keyID) {
	return engine->inputManager_->GetControllerStateNone(keyID);
}

bool Input::Controller::GetTrigger(uint8_t keyID) {
	return engine->inputManager_->GetControllerStateTrigger(keyID);
}

bool Input::Controller::GetPress(uint8_t keyID) {
	return engine->inputManager_->GetControllerStatePress(keyID);
}

bool Input::Controller::GetRelease(uint8_t keyID) {
	return engine->inputManager_->GetControllerStateRelease(keyID);
}