#include "Window.h"

using namespace LWP::System;
using namespace LWP;

void Window::ChangeWindowMode() {
	Base::WinApp::GetInstance()->ChangeWindowMode();
}
void Window::ChangeFullScreenMode() {
	Base::WinApp::GetInstance()->ChangeFullScreenMode();
}
void Window::ChangeBorderlessWindowMode() {
	Base::WinApp::GetInstance()->ChangeBorderlessWindowMode();
}