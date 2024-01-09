#pragma once
#include <Adapter.h>

class HandGun {
public: // メンバ関数

	// 初期化
	void Initialize(LWP::Object::WorldTransform* worldTransform);
	// 更新
	void Update();

	// レティクルのワールド座標を取得
	LWP::Math::Vector3 GetReticleWorldPosition() { return reticlePosition.GetWorldPosition(); }
	
	// 射撃
	bool Shot(LWP::Math::Vector3* recoil);
	int32_t GetAmmo() { return ammo_; } // 現在の装弾数を取得
	// ADS
	void SwitchADS();
	bool GetisADS() { return isADS; }
	// リロード
	void Reload();

private: // メンバ変数
	// 本体のモデル
	LWP::Primitive::Mesh* bodyModel_ = nullptr;
	LWP::Primitive::Mesh* sliderModel_ = nullptr;

	// サイトやレティクル用のUI
	LWP::Object::WorldTransform reticlePosition;
	LWP::Primitive::Sprite* reticleSprite = nullptr;
	LWP::Primitive::Sprite* redDotSprite = nullptr;

	//enum SENum {
	//	Fire,
	//	Reload1,
	//	Reload2,
	//	EmptyAmmo
	//};
	//// サウンドデータハンドル
	//uint32_t soundDataHandle_[5] = {0};

	// 装弾数表示用スプライト
	LWP::Primitive::Sprite* secondDigitNumberSprite_[2];	// 10の位
	LWP::Primitive::Sprite* firstDigitNumberSprite_[10];	// 1の位
	LWP::Primitive::Sprite* slashSprite_;
	LWP::Primitive::Sprite* maxAmmoNumSprite_[2];	// 最大装弾数を表示するためのスプライト
	// 装弾数
	int32_t kMAXAmmo_ = 13;
	// 弾数
	int32_t ammo_ = kMAXAmmo_;

	// ADS切り替えフラグ
	bool isADS = false;

	// 垂直リコイルパワー
	LWP::Math::Vector3 verticalRecoilPower_ = {-1.0f, 0.0f, 0.0f};
	// 水平ランダムリコイルパワー
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
	LWP::Math::Vector3 kADSTranslation_ = { 0.0f, 0.0f, 0.0f };
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