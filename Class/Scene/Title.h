#pragma once
#include <scene/IScene.h>
#include <Adapter.h>

/// <summary>
/// タイトルシーン
/// </summary>
class Title :
	public IScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Title() = default;

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
