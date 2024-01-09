#include "Skydome.h"

void Skydome::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	// モデル
	model_ = model;

	// ワールドトランスフォ－ムの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {500, 500, 500};

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Skydome::Update() {

}

void Skydome::Draw(const ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_); 
}