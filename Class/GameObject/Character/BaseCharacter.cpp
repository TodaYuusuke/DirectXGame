#include "BaseCharacter.h"

// 初期化
void BaseCharacter::Initialize(const std::vector<Model*>& models, Vector3 position) {
	models_.clear();

	for (int i = 0; i < models.size(); i++) {
		models_.insert(std::make_pair(i, ModelData()));
		models_[i].model_ = models[i];
		models_[i].worldTransform_.Initialize();
	}

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}
// 更新
void BaseCharacter::Update() {
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < models_.size(); i++) {
		models_[i].worldTransform_.UpdateMatrix();
	}
}
// 描画
void BaseCharacter::Draw(const ViewProjection& viewProjection) {
	for (int i = 0; i < models_.size(); i++) {
		models_[i].model_->Draw(models_[i].worldTransform_, viewProjection);
	}
}