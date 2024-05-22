#include <Adapter.h>
//#include "scene/Title.h"
#include "scene/Test.h"
#include "scene/GameScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp("LWP Sample Game");
	LWP::System::Run(new GameScene);
	return 0;
}