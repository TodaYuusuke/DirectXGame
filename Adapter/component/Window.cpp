#include "Window.h"

using namespace LWP::System;
using namespace LWP;

void Window::ChangeWindowMode() {
	engine->winApp_->ChangeWindowMode();
}
void Window::ChangeFullScreenMode() {
	engine->winApp_->ChangeFullScreenMode();
}
void Window::ChangeBorderlessWindowMode() {
	engine->winApp_->ChangeBorderlessWindowMode();
}