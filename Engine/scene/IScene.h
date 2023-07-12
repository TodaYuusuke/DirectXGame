#pragma once
#include "../../Adapter/LWP.h"

class IScene {
public:
	//*** 純粋仮想関数 ***//

	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() = 0;


	// 次のシーン（nullの間はシーン遷移しない）
	IScene* nextScene_ = nullptr;
};