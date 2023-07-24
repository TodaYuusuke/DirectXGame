#pragma once
#include "../../Adapter/LWP.h"
#include "../object/user/ObjectList.h"

class IScene {
public:

	// 共通の初期化
	virtual void PreInitialize() final {
		mainCamera = LWP::Engine::CreateObjectInstance<LWP::Object::Camera>();
		LWP::Engine::SetMainCamera(mainCamera);
	};

	//*** 純粋仮想関数 ***//

	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() = 0;


	// 次のシーン（nullの間はシーン遷移しない）
	IScene* nextScene_ = nullptr;

	// シーンが持つデフォルトのカメラ
	LWP::Object::Camera* mainCamera = nullptr;
};