#include <Adapter.h>
#include "Class/Scene/Title.h"
//#include "Engine/scene/user/SampleTitle.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::Run(new Title());
	return 0;
}