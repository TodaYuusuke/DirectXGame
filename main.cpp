#include <Adapter.h>
//#include "scene/user/SampleTitle.h"
#include "Class/Scene/GamePlay.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::Run(new GamePlay());
	return 0;
}