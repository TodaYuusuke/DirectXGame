#include <Adapter.h>
#include "Engine/scene/user/TItle.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::Run(new Title);
	return 0;
}