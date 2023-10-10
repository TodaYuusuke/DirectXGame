#include "Input.h"

using namespace LWP::System;
using namespace LWP;

bool Input::GetNone(uint8_t keyID) {
	return engine->inputManager_->GetKeyStateNone(keyID);
}

bool Input::GetTrigger(uint8_t keyID) {
	return engine->inputManager_->GetKeyStateTrigger(keyID);
}

bool Input::GetPress(uint8_t keyID) {
	return engine->inputManager_->GetKeyStatePress(keyID);
}

bool Input::GetRelease(uint8_t keyID) {
	return engine->inputManager_->GetKeyStateRelease(keyID);
}