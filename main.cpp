#include <Adapter.h>
#include "Engine/scene/user/SampleTitle.h"
#include "Engine/scene/user/SampleGameScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp(L"Window Title");
	LWP::System::Run(new SampleGameScene);
	return 0;
}