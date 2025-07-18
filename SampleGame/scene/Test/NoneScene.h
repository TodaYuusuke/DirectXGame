#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

/// <summary>
/// 何もないシーン
/// </summary>
class NoneScene final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;

};