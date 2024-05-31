#include <Adapter.h>
//#include "scene/Title.h"
#include "scene/Test.h"
#include "scene/GameScene.h"
#include "scene/ColliderTest.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp("LWP Sample Game");
	LWP::System::Run(new ColliderTest);
	return 0;
}