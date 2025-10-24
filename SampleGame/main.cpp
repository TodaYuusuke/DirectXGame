#include <Adapter.h>
//#include "scene/Title.h"
//#include "scene/GameScene.h"

#include "scene/Test/TestIncludes.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::Run<Collision3DTest>(L"LWP サンプルゲーム");
	return 0;
}