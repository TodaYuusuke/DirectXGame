#include "SceneManager.h"
#include "IScene.h"

using namespace LWP::Scene;

// 初期化
void Manager::Initialize(IScene* firstScene) {
	currentScene_ = firstScene;
	currentScene_->PreInitialize();
	currentScene_->Initialize();
}

// 更新
void Manager::Update() {
	// 次のシーンへ
	if (currentScene_->nextSceneFunction != nullptr) {
		// シーンクリア（仮置きなのでそのうち消去する)
		LWP::System::engine->InitializeForScene();

		// 関数実行
		IScene* temp = currentScene_->nextSceneFunction();
		delete currentScene_;
		currentScene_ = temp;
		// 初期化処理
		currentScene_->PreInitialize();
		currentScene_->Initialize();
	}

	currentScene_->Update();
}

void Manager::DebugGUI() {
	currentScene_->DebugGUI();
}


LWP::Object::Camera* Manager::GetMainCamera() {
	return currentScene_->GetMainRenderCamera();
}
