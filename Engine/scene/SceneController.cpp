#include "SceneController.h"
#include "Title.h"

using namespace LWP::Scene;

// 初期化
void Controller::Initialize() {
	currentScene_ = new Title();
	currentScene_->Initialize();
}
// 更新
void Controller::Update() {
	currentScene_->Update();
	if (currentScene_->nextScene_ != nullptr) {
		currentScene_ = currentScene_->nextScene_;
		currentScene_->Initialize();
	}
}
// 描画
void Controller::Draw() {
	currentScene_->Draw();
}