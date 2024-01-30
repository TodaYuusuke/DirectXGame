#include "Skydome.h"

void Skydome::Initialize() {
	// モデル
	model_ = LWP::Resource::LoadModel("skydome/skydome.obj");

	// ワールドトランスフォ－ムの初期化
	model_->transform.scale = { 500, 500, 500 };
	//model_->isActive = false;
}

void Skydome::Update() {

}