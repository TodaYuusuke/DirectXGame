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