#include "System.h"

using namespace LWP;

void System::ShutDown() {
	DestroyWindow(Base::WinApp::GetInstance()->GetHWND());
}