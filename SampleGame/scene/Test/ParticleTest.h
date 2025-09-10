#pragma once
#include "particle/DirtSplash.h"
#include "particle/TracesofDirt.h"
#include "scene/IScene.h"

class ParticleTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	// パーティクルのクラス
	DirtSplash dirt_;
	TracesofDirt tracesOfDirt_;
};