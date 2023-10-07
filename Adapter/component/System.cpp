#include "System.h"

using namespace LWP;

void System::Run() {
	// エンジン生成
	engine = std::make_unique<Engine>();
	// エンジン起動
	engine->Run();
}


// 静的変数の実態
std::unique_ptr<System::Engine> System::engine;