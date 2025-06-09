#include "Information.h"

using namespace LWP::System;
using namespace LWP::Base;
using namespace LWP;

int Information::GetWindowWidth() {
	return WinApp::GetInstance()->GetClientWidth();
}

int Information::GetWindowHeight() {
	return WinApp::GetInstance()->GetClientHeight();
}

float Information::GetWindowWidthF() {
	return static_cast<float>(WinApp::GetInstance()->GetClientWidth());
}

float Information::GetWindowHeightF(){
	return static_cast<float>(WinApp::GetInstance()->GetClientHeight());
}

Math::Vector2 Information::GetWindow() {
	return {
		static_cast<float>(WinApp::GetInstance()->GetClientWidth()),
		static_cast<float>(WinApp::GetInstance()->GetClientHeight())
	};
}

double Information::GetDeltaTime() {
	return FrameTracker::GetInstance()->GetDeltaTime();
}
float Information::GetDeltaTimeF() {
	return static_cast<float>(GetDeltaTime());
}
double Information::GetDefaultDeltaTime() {
	return FrameTracker::GetInstance()->GetDefaultDeltaTime();
}
float Information::GetDefaultDeltaTimeF() {
	return static_cast<float>(GetDefaultDeltaTime());
}

void Information::SetDeltaTimeMultiply(float value) {
	FrameTracker::GetInstance()->SetTimeScale(value);
}

// 実態を定義
#if DEMO
bool Information::isDebugGUIVisible = true;
#else
bool Information::isDebugGUIVisible = false;
#endif