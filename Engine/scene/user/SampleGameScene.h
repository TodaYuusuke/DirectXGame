#pragma once
#include "../IScene.h" 

class SampleGameScene final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//
	LWP::Primitive::Sphere sphere;
	LWP::Resource::Motion motion;
};