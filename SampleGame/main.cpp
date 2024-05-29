#include <Adapter.h>
#include "scene/Title.h"
#include "scene/Test.h"
#include "scene/GameScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp(L"LWP サンプル Game");
	LWP::System::Run(new Test);
	return 0;
}