#pragma once
#include "../IScene.h" 
#include "../../../Class/Map/MapManager.h"
#include "../../../Class/Hammer/Hammer.h"
#include "../../../Class/Mob/MobManager.h"
#include "../Class/Skydome/Skydome.h"

class GameScene final
	: public IScene {
public:

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//

	// マップ管理クラス
	MapManager mapManager_;
	// ハンマー（プレイヤー）
	Hammer hammer_;
	// スカイドーム
	Skydome skydome_;
	// モブ管理クラス
	MobManager mobManager_;

	// サウンド
	LWP::Resource::Audio* gameBGM_ = nullptr;
	float bgmVol_ = 0.0f;

	// 数値テクスチャ
	LWP::Resource::Texture* numberTex_[10] = { nullptr };
	// スコア
	int32_t score_;
	float scoreEaseT;
	int32_t calcScore_[2];
	int32_t viewScore_;
	LWP::Primitive::Surface* scoreUI_[8] = { nullptr };
	// タイム
	int32_t time_;
	LWP::Primitive::Surface* timeUI_[3] = { nullptr };
	int32_t nextSceneWaitTime_;

	// クリア判定フラグ
	bool isClear_;


private://***** カメラ移動  *****//

	LWP::Math::Vector2 RStickVelocity;

	void Reset();

	LWP::Math::Vector3 Lerp(const LWP::Math::Vector3& start, const LWP::Math::Vector3& end, float t);


private://*** チュートリアル ***//

	bool Tutorial;
	bool ControrllTutorial;
	bool ShockWaveTutorial;
	bool EnemyKillTutorial;

};