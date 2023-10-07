#pragma once
#include "../../Adapter/Adapter.h"

class IScene {
public:
	// 共通の初期化
	void PreInitialize() {
		mainCamera = LWP::Object::CreateInstance<LWP::Object::Camera>();
		mainCamera->transform.translation = { 0.0f,0.0f,-5.0f };
	};

	//*** 純粋仮想関数 ***//

	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() = 0;

	// ** メンバ変数 ** //

	// 次のシーン（nullの間はシーン遷移しない）
	IScene* nextScene_ = nullptr;

	// シーンが持つデフォルトのカメラ
	LWP::Object::Camera* mainCamera = nullptr;
};
