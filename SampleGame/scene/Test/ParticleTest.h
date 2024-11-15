#pragma once
#include "class/particle/TerrainParticle.h"
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
	TerrainParticle particle_;

};