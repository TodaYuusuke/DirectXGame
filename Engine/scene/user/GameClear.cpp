#include "GameClear.h"
#include "TItle.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void GameClear::Initialize() {

	// テクスチャ読み込み
	transitionTex_ = LWP::Resource::LoadTexture("Transition.png");
	
	// 平面
	transitionSurfase_ = LWP::Primitive::CreateInstance<Surface>();
	transitionSurfase_->vertices[0].position = { 0.0f,0.0f };
	transitionSurfase_->vertices[1].position = { 320.0f,0.0f };
	transitionSurfase_->vertices[2].position = { 320.0f,180.0f };
	transitionSurfase_->vertices[3].position = { 0.0f,180.0f };
	transitionSurfase_->isUI = true;
	transitionSurfase_->isActive = true;
	transitionSurfase_->texture = transitionTex_;

}

// 更新
void GameClear::Update() {

	// ENTERキーを押すとシーン切り替え
	if (LWP::Input::GetTrigger(DIK_RETURN)) {
		nextScene_ = new Title();
	}

	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();

}

// 描画
void GameClear::Draw() {

}
