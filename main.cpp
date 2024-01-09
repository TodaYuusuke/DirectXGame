#include <Adapter.h>
#include "scene/user/SampleTitle.h"
//#include "Class/Scene/Title.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	LWP::System::Run(new SampleTitle());
	return 0;
}