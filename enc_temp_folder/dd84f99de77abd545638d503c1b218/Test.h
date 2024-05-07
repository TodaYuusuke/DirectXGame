#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

class Test final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	Test() : indexManager(30) {};

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Primitive::Mesh mesh;
	// アニメーション
	LWP::Resource::Animation idleAnim[4];

	LWP::Utility::IndexManager indexManager;
	std::vector<LWP::Utility::Index> indexes;
};