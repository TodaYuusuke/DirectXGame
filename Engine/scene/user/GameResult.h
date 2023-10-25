#pragma once
#include "../IScene.h"

class GameResult final
	: public IScene {
public:
	GameResult(int32_t score, int32_t time);
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
	LWP::Resource::Texture* backgroundTex_ = nullptr;
	LWP::Resource::Texture* transitionTex_ = nullptr;
	LWP::Resource::Texture* buttonTex_ = nullptr;

	// モデル

	//	背景用の平面
	LWP::Primitive::Surface* background_ = nullptr;

	// シーン遷移用の平面
	LWP::Primitive::Surface* transitionSurfase_[2] = { nullptr };

	// リザルト用
	LWP::Primitive::Surface* titleButton_ = nullptr;
	LWP::Primitive::Surface* retriButton_ = nullptr;

	// イージング用ワールドトランスフォーム
	LWP::Object::WorldTransform titleWorldTransform_[2];
	LWP::Object::WorldTransform retriWorldTransform_[2];

	// イージング用タイム
	float t[3] = { 0.0f };

	// 数値テクスチャ
	LWP::Resource::Texture* numberTex_[10] = { nullptr };
	// スコア
	int32_t score_;
	int32_t viewScore_;
	LWP::Primitive::Surface* scoreUI_[8] = { nullptr };
	// タイム
	int32_t time_;
	int32_t viewTime;
	LWP::Primitive::Surface* timeUI_[3] = { nullptr };
	int32_t nextSceneWaitTime_;

	// タイトルかリトライの選択ボタン
	bool isGoTitle = false;

	// シーン遷移のフラグ(シーン遷移時にtrue)
	bool isTransition_ = false;
	bool isTransitionSceneEnd_ = false;

};
