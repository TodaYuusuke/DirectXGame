#include "Ground.h"

void Ground::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	// モデル
	model_ = model;

	// ワールドトランスフォ－ムの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {50, 50, 50};

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Ground::Update() {

}

void Ground::Draw(const ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_); 
}