#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

/// <summary>
/// 影チェック用
/// </summary>
class ShadowTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: // ** メンバ変数 ** //

	LWP::Resource::RigidModel axis;
	LWP::Resource::RigidModel field;
	LWP::Resource::RigidModel build[5];

	LWP::Resource::RigidModel sphere;
};