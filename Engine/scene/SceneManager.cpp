#include "SceneManager.h"
#include "IScene.h"

using namespace LWP::Scene;


Manager::~Manager() {
	// 現在のシーンを削除
	if (currentScene_ != nullptr) {
		delete currentScene_;
		currentScene_ = nullptr;
	}
}

// 初期化
void Manager::Init(IScene* firstScene) {
	currentScene_ = firstScene;
	currentScene_->PreInitialize();
	currentScene_->Initialize();
}

// 更新
void Manager::Update() {
	// 次のシーンへ
	if (currentScene_->nextSceneFunction != nullptr) {
		// シーンクリア（仮置きなのでそのうち消去する)
		SceneClear();

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

void Manager::SceneClear() {
	Object::Manager::GetInstance()->Init();			// Objectのオブジェクトをクリア
	Primitive::Manager::GetInstance()->Init();		// Primitiveのオブジェクトをクリア
	Object::CollisionManager::GetInstance()->Init();// CollisionManagerのオブジェクトをクリア
	Input::Manager::GetInstance()->ClearInput();	// Inputの入力をクリア
}

void Manager::DebugGUI() {
	currentScene_->DebugGUI();
}
