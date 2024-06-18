#include "Information.h"

using namespace LWP::System;
using namespace LWP;

int Information::GetWindowWidth() {
	return engine->winApp_->GetClientWidth();
}

int Information::GetWindowHeight() {
	return engine->winApp_->GetClientHeight();
}

float Information::GetWindowWidthF() {
	return static_cast<float>(engine->winApp_->GetClientWidth());
}

float Information::GetWindowHeightF(){
	return static_cast<float>(engine->winApp_->GetClientHeight());
}

Math::Vector2 Information::GetWindow() {
	return {
		static_cast<float>(engine->winApp_->GetClientWidth()),
		static_cast<float>(engine->winApp_->GetClientHeight())
	};
}

void Information::ChangeShowDebugGUI() {
	engine->isShowDebugGUI = !engine->isShowDebugGUI;
}

double Information::GetDeltaTime() {
	return engine->debugTimer_.GetDeltaTime();
}
float Information::GetDeltaTimeF() {
	return static_cast<float>(GetDeltaTime());
}
double Information::GetDefaultDeltaTime() {
	return engine->debugTimer_.GetDefaultDeltaTime();
}
float Information::GetDefaultDeltaTimeF() {
	return static_cast<float>(GetDefaultDeltaTime());
}

void Information::SetDeltaTimeMultiply(float value) {
	engine->debugTimer_.SetDeltaTimeMultiply(value);
}
