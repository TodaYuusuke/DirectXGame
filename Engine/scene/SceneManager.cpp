#include "SceneManager.h"
#include "IScene.h"
#include "user/Title.h"

using namespace LWP::Scene;

// 初期化
void Manager::Initialize() {
	currentScene_ = new Title();
	currentScene_->PreInitialize();
	currentScene_->Initialize();
}
// 更新
void Manager::Update() {
	if (currentScene_->nextScene_ != nullptr) {
		//delete currentScene_;
		currentScene_ = currentScene_->nextScene_;
		LWP::System::engine->InitializeForScene();
		currentScene_->PreInitialize();
		currentScene_->Initialize();
	}

	currentScene_->Update();
}
// 描画
void Manager::Draw() {
	currentScene_->Draw();
}

const LWP::Object::Camera* Manager::GetMainCamera() {
	return currentScene_->mainCamera;
}
