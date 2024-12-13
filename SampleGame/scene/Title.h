#pragma once
#include "../class/field/Field.h"
#include "scene/IScene.h"

class Title final
	: public IScene {
public:
	Title() = default;
	~Title() = default;

	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	// 地形データ
	Field field_;
	// このシーン専用のモデル
	LWP::Resource::SkinningModel drone_;
	LWP::Resource::Animation droneAnim_;

	// 操作表示
	LWP::Primitive::Sprite ui_;

	// 画面遷移用UI
	LWP::Primitive::Sprite stagingSprite_;
	// シーン開始演出用
	struct StartStaging {
		// 経過時間
		float time = 0.0f;
		// アニメーションにかかる時間
		float kTime = 2.0f;

		// 終了フラグ
		bool end = false;
	}startStaging_;
	// シーン終了演出用
	struct EndStaging {
		// 経過時間
		float time = 0.0f;
		// アニメーションにかかる時間
		float kTime = 1.5f;

		// カメラのアニメーション
		LWP::Math::Vector3 startPos{};	// カメラの初期座標
		LWP::Math::Quaternion startRotation{};	// カメラの初期角度

		// 開始フラグ
		bool start = false;
		// 終了フラグ
		bool end = false;
	}endStaging_;
};