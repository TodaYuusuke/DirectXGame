#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

// IndexManagerのバグ回避用に何もないシーンを作成
class NullScene final
	: public IScene {
public:
	NullScene(std::function<IScene*()>);
	~NullScene() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override {};
	// 更新
	void Update() override {};


private: //*** これより先に必要な処理や変数を記述 ***//

	LWP::Primitive::NormalSprite sprite_;
};