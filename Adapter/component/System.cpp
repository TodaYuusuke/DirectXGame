#include "System.h"

using namespace LWP;

void System::Run() {
	// エンジン生成
	engine_ = std::make_unique<Engine>();
	// エンジン起動
	engine_->Run();
}


// 静的変数の実態
std::unique_ptr<LWP::System::Engine> LWP::System::engine_;