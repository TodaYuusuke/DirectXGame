#include "SceneManager.h"
#include "user/TItle.h"

using namespace LWP::Scene;

// 初期化
void Manager::Initialize() {
	currentScene_ = new Title();
	currentScene_->PreInitialize();
	currentScene_->Initialize();
}
// 更新
void Manager::Update() {
	currentScene_->Update();
	if (currentScene_->nextScene_ != nullptr) {
		currentScene_ = currentScene_->nextScene_;
		currentScene_->Initialize();
	}

	// カメラのビュープロジェクションをPrimitiveManagerに登録
}
// 描画
void Manager::Draw() {
	currentScene_->Draw();
}