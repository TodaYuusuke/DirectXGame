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
	if (currentScene_->nextScene_ != nullptr) {
		//delete currentScene_;
		currentScene_ = currentScene_->nextScene_;
		
		// 仮置きなのでそのうち消去する
		LWP::System::engine->InitializeForScene();
		
		currentScene_->PreInitialize();
		currentScene_->Initialize();
	}

	currentScene_->Update();
}

const LWP::Object::Camera* Manager::GetMainCamera() {
	return currentScene_->GetMainRenderCamera();
}
