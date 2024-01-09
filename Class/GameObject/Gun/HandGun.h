#pragma once
#include "Model.h"
#include <Sprite.h>
#include <Audio.h>

class HandGun {
public: // メンバ関数

	// 初期化
	void Initialize(const WorldTransform& worldTransform);
	// 更新
	void Update();
	// 描画
	void Draw(const ViewProjection& viewProjection);
	void DrawUI();

	// レティクルのワールド座標を取得
	Vector3 GetReticleWorldPosition() { return reticleWorldTransform.GetWorldPosition(); }
	
	// 射撃
	bool Shot(Vector3* recoil);
	int32_t GetAmmo() { return ammo_; } // 現在の装弾数を取得
	// ADS
	void SwitchADS();
	bool GetisADS() { return isADS; }
	// リロード
	void Reload();

private: // メンバ変数
	std::unique_ptr<Model> bodyModel_;
	std::unique_ptr<Model> sliderModel_;
	WorldTransform bodyWT_;
	WorldTransform sliderWT_;

	WorldTransform reticleWorldTransform;
	std::unique_ptr<Sprite> reticleSprite = nullptr;
	std::unique_ptr<Sprite> redDotSprite = nullptr;

	enum SENum {
		Fire,
		Reload1,
		Reload2,
		EmptyAmmo
	};
	// オーディオクラスのインスタンス
	Audio* audio_ = nullptr;
	// サウンドデータハンドル
	uint32_t soundDataHandle_[5] = {0};

	// 装弾数表示用スプライト
	std::unique_ptr<Sprite> secondDigitNumberSprite_[2];	// 10の位
	std::unique_ptr<Sprite> firstDigitNumberSprite_[10];	// 1の位
	std::unique_ptr<Sprite> slashSprite_;
	std::unique_ptr<Sprite> maxAmmoNumSprite_[2];	// 最大装弾数を表示するためのスプライト
	// 装弾数
	int32_t kMAXAmmo_ = 13;
	// 弾数
	int32_t ammo_ = kMAXAmmo_;

	// ADS切り替えフラグ
	bool isADS = false;

	// 垂直リコイルパワー
	Vector3 verticalRecoilPower_ = {-1.0f, 0.0f, 0.0f};
	// 水平ランダムリコイル
	float horizontalRecoilPower_ = 0.3f;

private: // プライベートな関数
	// モデルをニュートラルにセット
	void SetModelNeutral();
	// 調整項目適応
	void ApplyGlobalVariables();

private: // アニメーション関連

	// リコイルのパラメータ
	int32_t kRecoilFrame_ = 10;
	float kRecoilRotationX_ = -0.8f;
	float kRecoilSliderTranslationZ_ = -2.0f;
	int32_t recoilFrame_ = -1;
	void RecoilAnimation();

	// ADSのパラメータ
	int32_t kADSFrame_ = 10;
	Vector3 kADSTranslation_ = {0.0f, 0.0f, 0.0f};
	int32_t adsFrame_ = 0;
	void ADSAnimation();

	// リロードのパラメータ
	int32_t kReloadIOFrame_ = 30;
	int32_t kReloadWaitFrame_ = 30;
	float kReloadTranslationY_ = -1.0f;
	float kReloadRotationX_ = 1.75f;
	int32_t reloadFrame_ = -1;
	void ReloadAnimation();
};