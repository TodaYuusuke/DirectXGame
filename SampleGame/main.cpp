#include <Adapter.h>
#include "scene/Title.h"
#include "scene/GameScene.h"

#include "scene/Test/TestIncludes.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::StartUp(L"LWP サンプルゲーム");
#if DEMO
#else
	// デバッグ情報の表示切替フラグ
	LWP::Window::ChangeFullScreenMode();	// フルスクリーン
	//LWP::Info::ChangeShowDebugGUI();
#endif
	LWP::System::Run(new SkinningTest);
	return 0;
}