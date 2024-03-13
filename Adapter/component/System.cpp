#include "System.h"

using namespace LWP;

void System::StartUp(std::string windowTitle) {
	// エンジン生成
	engine = std::make_unique<Engine>();
	engine->StartUp(windowTitle);

	// ISceneのインスタンスを作る際に、エンジンのクラスのポインタをエンジンに送ろうとするが、
	// エンジンの生成より前に行われると困るので、初期化と起動の関数を分けている
}
void System::Run(IScene* firstScene) {
	// エンジン起動
	engine->Run(firstScene);
}

void System::End() {
	DestroyWindow(engine->winApp_->GetHWND());
}


// 静的変数の実態
std::unique_ptr<System::Engine> System::engine;