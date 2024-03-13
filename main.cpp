#include <Adapter.h>
#include "Engine/scene/user/SampleGameScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp("Window Title");
	LWP::System::Run(new SampleGameScene);
	return 0;
}