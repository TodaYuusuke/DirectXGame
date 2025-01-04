#include <Adapter.h>
#include "scene/Title.h"
#include "scene/GameScene.h"

#include "scene/Test/TestIncludes.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp(L"LWP サンプルゲーム");
#if DEMO
#else
	LWP::Window::ChangeFullScreenMode();	// フルスクリーン
#endif
	LWP::System::Run(new Title);
	return 0;
}