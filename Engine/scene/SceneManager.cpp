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
	// 次のシーンへ差し替え
	if (currentScene_->nextScene_ != nullptr) {
		IScene* temp = currentScene_->nextScene_;
		delete currentScene_;
		currentScene_ = temp;
		
		// 仮置きなのでそのうち消去する
		LWP::System::engine->InitializeForScene();
	}

	currentScene_->Update();

	// 次のシーンの初期化だけ行う
	if (currentScene_->nextScene_ != nullptr) {
		currentScene_->nextScene_->PreInitialize();
		currentScene_->nextScene_->Initialize();
	}
}

LWP::Object::Camera* Manager::GetMainCamera() {
	return currentScene_->GetMainRenderCamera();
}
