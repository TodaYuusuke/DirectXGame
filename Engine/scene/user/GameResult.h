#pragma once
#include "../IScene.h"

class GameResult final
	: public IScene {
public:
	GameResult() = default;
	~GameResult() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// テクスチャ
	LWP::Resource::Texture* transitionTex_ = nullptr;
	LWP::Resource::Texture* buttonTex_ = nullptr;

	// モデル


	// シーン遷移用の平面
	LWP::Primitive::Surface* transitionSurfase_[2] = { nullptr };

	// リザルト用
	LWP::Primitive::Surface* resultScore_ = nullptr;
	LWP::Primitive::Surface* resultTime_ = nullptr;
	LWP::Primitive::Surface* result_ = nullptr;

	// イージング用ワールドトランスフォーム
	LWP::Object::WorldTransform scoreWorldTransform_[2];
	LWP::Object::WorldTransform timeWorldTransform_[2];
	LWP::Object::WorldTransform resultWorldTransform_[2];

	// イージング用タイム
	float t[3] = { 0.0f };

	// タイトルかリトライの選択ボタン
	bool isGoTitle = false;

	// シーン遷移のフラグ(シーン遷移時にtrue)
	bool isTransition_ = false;
	bool isTransitionSceneEnd_ = false;

};
