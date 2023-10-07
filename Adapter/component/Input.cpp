#include "Input.h"

using namespace LWP::System;
using namespace LWP;

void Input::GetNone(uint8_t keyID) {
	engine->inputManager_->GetKeyStateNone(keyID);
}

void Input::GetTrigger(uint8_t keyID) {
	engine->inputManager_->GetKeyStateTrigger(keyID);
}

void Input::GetPress(uint8_t keyID) {
	engine->inputManager_->GetKeyStatePress(keyID);
}

void Input::GetRelease(uint8_t keyID) {
	engine->inputManager_->GetKeyStateRelease(keyID);
}