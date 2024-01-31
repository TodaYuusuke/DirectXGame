#pragma once
#include <scene/IScene.h>
#include <Adapter.h>

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOver :
	public IScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameOver() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;


private: // メンバ変数

	LWP::Primitive::Sprite* sprite_;
};