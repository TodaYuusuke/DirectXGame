#include <Adapter.h>
#include "Class/Scene/GameScene.h"
//#include "Engine/scene/user/SampleTitle.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::Run(new GameScene);
	return 0;
}