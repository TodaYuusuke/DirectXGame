#include "FPSCamera.h"
#include <ImGuiManager.h>

void FPSCamera::Initialize() {
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	rotation_ = {0.0f, 0.0f, 0.0f};
}

void FPSCamera::Update() {
	// 追従対象がいれば
	if (target_) {
		// 座標と回転をコピーする
		viewProjection_.translation_ = target_->translation_;
		viewProjection_.rotation_ = target_->rotation_;
	}

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.UpdateViewMatrix();
}