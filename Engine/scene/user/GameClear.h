#pragma once
#include "../IScene.h"

class GameClear final
	: public IScene {
public:
	GameClear() = default;
	~GameClear() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// シーン遷移用の平面
	LWP::Primitive::Surface* transitionSurfase_;
	// シーン遷移用のテクスチャ
	LWP::Resource::Texture* transitionTex_;
	// シーン遷移用のワールドトランスフォーム
	LWP::Object::WorldTransform* transitionWorldTransform_;


};
