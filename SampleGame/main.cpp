#include <Adapter.h>
#include "scene/Title.h"
#include "scene/GameScene.h"

#include "scene/Test/TestIncludes.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#if DEMO
#else
	LWP::Window::ChangeFullScreenMode();	// フルスクリーン
#endif
	LWP::System::Run<ColliderTest>(L"LWP サンプルゲーム");
	return 0;
}